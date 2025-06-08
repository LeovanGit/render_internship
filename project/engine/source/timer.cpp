#include "timer.hpp"

namespace
{
using namespace std::chrono;
} // namespace

namespace engine
{
Timer::Timer()
{
    restart();
}

float Timer::getElapsedTime()
{
    return TimeSystem::getTimePoint() - start_time;
}

void Timer::restart()
{
    start_time = TimeSystem::getTimePoint();
}
} // namespace engine
