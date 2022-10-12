#include "random.hpp"

namespace math
{
float randomFromRange(float min, float max)
{
    std::random_device random_device;
    std::mt19937 mersenne_random(random_device());
    
    std::uniform_real_distribution<float> distribution(min, max);
    return distribution(random_device);
}
} // namespace math
