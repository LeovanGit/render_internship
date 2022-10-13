#ifndef RANDOM_HPP
#define RANDOM_HPP

#include <random>
#include <vector>
#include "glm.hpp"

#include "constants.hpp"

namespace math
{
float randomFromRange(float min, float max);

// r - min distance between two samples
// k - limit of samples to choose before rejection
std::vector<glm::vec2> poissonDiscSampling(uint32_t width,
                                           uint32_t height,
                                           float radius,
                                           uint32_t k = 30);
} // namespace math

#endif
