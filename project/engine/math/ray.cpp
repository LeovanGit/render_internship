#include "ray.hpp"

math::Ray::Ray(const glm::vec3 & origin,
               const glm::vec3 & direction) :
               origin(origin),
               direction(direction)
{}

