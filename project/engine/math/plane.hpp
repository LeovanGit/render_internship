#ifndef PLANE_HPP
#define PLANE_HPP

#include "glm.hpp"

#include "ray.hpp"
#include "intersection.hpp"
#include "euler_angles.hpp"
#include "constants.hpp"

namespace math
{
class Plane
{
public:
    Plane(const glm::vec3 & normal,
          const glm::vec3 & origin);

    bool intersect(math::Intersection & nearest,
                   const math::Ray & ray);

    glm::vec3 normal;
    glm::vec3 origin;
};
} // namespace math

#endif
