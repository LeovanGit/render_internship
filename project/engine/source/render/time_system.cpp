#include "time_system.hpp"

namespace engine
{
std::chrono::steady_clock::time_point TimeSystem::init_time;

void TimeSystem::init()
{    
    init_time = std::chrono::steady_clock::now();
}

const std::chrono::steady_clock::time_point & TimeSystem::getInitTime()
{
    return init_time;
}

float TimeSystem::getTimePoint()
{
    return (std::chrono::steady_clock::now() - init_time).count();
}
} // namespace engine
