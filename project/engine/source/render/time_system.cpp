#include "time_system.hpp"

namespace
{
using namespace std::chrono;
} // namespace

namespace engine
{
steady_clock::time_point TimeSystem::init_time;
steady_clock::time_point TimeSystem::pause_start;
bool TimeSystem::is_paused = false;

void TimeSystem::init()
{    
    init_time = steady_clock::now();
}

const steady_clock::time_point & TimeSystem::getInitTime()
{
    return init_time;
}

float TimeSystem::getTimePoint()
{
    duration<float> elapsed_time = steady_clock::now() - init_time;
    
    return elapsed_time.count();
}

void TimeSystem::pause()
{
    if (!is_paused)
    {
        is_paused = true;
        pause_start = steady_clock::now();
    }
}

void TimeSystem::unpause()
{
    if (is_paused)
    {
        is_paused = false;

        milliseconds pause_duration = duration_cast<milliseconds>(steady_clock::now() - pause_start);
        init_time += pause_duration;
    }
}
} // namespace engine
