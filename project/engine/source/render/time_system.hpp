#ifndef TIME_SYSTEM_HPP
#define TIME_SYSTEM_HPP

#include <chrono>

namespace engine
{
class TimeSystem final
{
public:
    // deleted methods should be public for better error messages
    TimeSystem() = delete;
    TimeSystem(const TimeSystem & other) = delete;
    void operator=(const TimeSystem & other) = delete;

    static void init();

    static const std::chrono::steady_clock::time_point & getInitTime();

    static float getTimePoint();

private:
    static std::chrono::steady_clock::time_point init_time;
};
} // namespace engine

#endif
