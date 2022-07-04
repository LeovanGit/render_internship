#ifndef PARALLEL_EXECUTOR_HPP
#define PARALLEL_EXECUTOR_HPP

#include <vector>
#include <atomic>
#include <functional>
#include <thread>
#include <shared_mutex>
#include <condition_variable>

#define BREAK __debugbreak();

#define ALWAYS_ASSERT(expression, ...) \
	if (!(expression)) \
	{ \
		BREAK; \
		std::abort(); \
	}

#ifdef NDEBUG
#define DEV_ASSERT(...)
#else
#define DEV_ASSERT(expression, ...) ALWAYS_ASSERT(expression, __VA_ARGS__);
#endif

struct ParallelExecutor
{
    // No point to have more threads than the number of CPU logical cores.
    // 100% CPU occupation, it may cause OS hitches.
    static const uint32_t MAX_THREADS;    
    // 50-100% CPU occupation
    static const uint32_t HALF_THREADS;

    // (threadIndex, taskIndex)
    using Func = std::function<void(uint32_t, uint32_t)>;

    ParallelExecutor(uint32_t numThreads);

    ~ParallelExecutor();

    uint32_t numThreads() const;

    bool isWorking() const;

    void wait();

    // Executes a function in parallel blocking the caller thread.
    void execute(const Func& func,
                 uint32_t numTasks,
                 uint32_t tasksPerBatch);

    // Executes a function in parallel asynchronously.
    void executeAsync(const Func& func,
                      uint32_t numTasks,
                      uint32_t tasksPerBatch);

protected:
    void awake();

    void loop(uint32_t threadIndex);

    bool m_isLooping;

    std::atomic<uint32_t> m_finishedThreadNum;
    std::atomic<uint32_t> m_completedBatchNum;
    std::function<void(uint32_t)> m_executeTasks;

    std::shared_mutex m_mutex;
    std::condition_variable_any m_waitCV;
    std::condition_variable_any m_workCV;

    std::vector<std::thread> m_threads;
};

#endif
