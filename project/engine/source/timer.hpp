#ifndef TIMER_HPP
#define TIMER_HPP

#include "time_system.hpp"

namespace engine
{
class Timer
{
public:
    Timer();

    float getElapsedTime();
    void restart();
    
private:
    float start_time;
};
} // namespace engine

#endif
