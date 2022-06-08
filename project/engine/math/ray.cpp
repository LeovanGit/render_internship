#include "ray.hpp"

math::Ray::Ray(const glm::vec3 & origin,
               const glm::vec3 & direction) :
               origin(origin),
               direction(direction)
{}

glm::vec3 math::Ray::reflect(const glm::vec3 & normal) const
{
    return 2.0f * glm::dot(direction, normal) * normal - direction;
}
