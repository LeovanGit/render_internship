#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>

namespace engine
{
class Timer
{
public:
    Timer();

    float getElapsedTime();
    void restart();
    
private:
    std::chrono::steady_clock::time_point start_time;
};
} // namespace engine

#endif
