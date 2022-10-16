#include "timer.hpp"

namespace
{
using namespace std::chrono;
} // namespace

namespace engine
{
Timer::Timer() { restart(); }

float Timer::getElapsedTime()
{
    duration<float> elapsed_time = steady_clock::now() - start_time;

    return elapsed_time.count();
}

void Timer::restart() { start_time = steady_clock::now(); }
} // namespace engine
