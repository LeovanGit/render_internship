#include "time_system.hpp"

namespace
{
using namespace std::chrono;
} // namespace

namespace engine
{
steady_clock::time_point TimeSystem::init_time;

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
    return duration_cast<milliseconds>(steady_clock::now() - init_time).count();
}
} // namespace engine
