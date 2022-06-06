#include "parallel_executor.hpp"

const uint32_t ParallelExecutor::MAX_THREADS = 
    std::max(1u, std::thread::hardware_concurrency());
const uint32_t ParallelExecutor::HALF_THREADS = 
    std::max(1u, std::thread::hardware_concurrency() / 2);

ParallelExecutor::ParallelExecutor(uint32_t numThreads)
{
    m_isLooping = true;

    DEV_ASSERT(numThreads > 0);
    m_finishedThreadNum = 0;

    m_threads.reserve(numThreads);
    for (uint32_t i = 0; i < numThreads; ++i)
        m_threads.emplace_back([this, i]() { loop(i); });
}

ParallelExecutor::~ParallelExecutor()
{
    wait();
    m_isLooping = false;
    awake();

    for (auto& t : m_threads) t.join();
}

uint32_t ParallelExecutor::numThreads() const { return m_threads.size(); }

bool ParallelExecutor::isWorking() const 
{ 
    return m_finishedThreadNum < m_threads.size(); 
}

void ParallelExecutor::wait()
{
    if (!isWorking()) return;

    std::unique_lock<std::shared_mutex> lock(m_mutex);

    // re-check for a case when threads finished and
    // m_waitCV is notified before the lock is acquired
    if (!isWorking()) return; 

    m_waitCV.wait(lock);
}

void ParallelExecutor::execute(const Func& func,
                               uint32_t numTasks,
                               uint32_t tasksPerBatch)
{
    if (numTasks == 0) return;
    executeAsync(func, numTasks, tasksPerBatch);
    wait();
}

void ParallelExecutor::executeAsync(const Func& func,
                                    uint32_t numTasks,
                                    uint32_t tasksPerBatch)
{
    if (numTasks == 0) return;
    DEV_ASSERT(tasksPerBatch > 0);

    // enforced waiting for completion of the previous parallel dispatch
    wait();

    m_finishedThreadNum = 0;
    m_completedBatchNum = 0;

    uint32_t numBatches = (numTasks + tasksPerBatch - 1) / tasksPerBatch;

    m_executeTasks = [this,
                      func,
                      numTasks,
                      numBatches,
                      tasksPerBatch](uint32_t threadIndex)
    {
        while (true)
        {
            uint32_t batchIndex = m_completedBatchNum.fetch_add(1);
            if (batchIndex >= numBatches) return;

            uint32_t begin = (batchIndex + 0) * tasksPerBatch;
            uint32_t end = (batchIndex + 1) * tasksPerBatch;
            if (end > numTasks) end = numTasks;

            for (uint32_t taskIndex = begin; taskIndex < end; ++taskIndex)
                func(threadIndex, taskIndex);
        }
    };

    awake();
}

void ParallelExecutor::awake()
{
    m_workCV.notify_all();
}

void ParallelExecutor::loop(uint32_t threadIndex)
{
    while (true)
    {
        {
            std::shared_lock<std::shared_mutex> lock(m_mutex);

            uint32_t prevFinishedNum = m_finishedThreadNum.fetch_add(1);
            if ((prevFinishedNum + 1) == m_threads.size())
            {
                m_executeTasks = {};
                m_waitCV.notify_all(); 
                // If an outer thread waits on m_waitCV, it will remain
                // blocked until this thread enters m_workCV.wait(),
                // because both CVs wait with the same m_mutex. This is
                // needed to avoid this thread missing
                // a notification on m_workCV in a situation when an outer
                // thread unblocks after this line and before this thread
                // enters m_workCV.wait(), which would result in this
                // thread being blocked until a next notification.
            }

            // Calling this unlocks m_mutex until m_workCV is notified,
            // then re-locks it again until the end of the scope.
            m_workCV.wait(lock);
        }

        if (!m_isLooping) return;

        DEV_ASSERT(m_executeTasks);
        m_executeTasks(threadIndex);
    }
}
