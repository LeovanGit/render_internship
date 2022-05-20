#ifndef PLANE_HPP
#define PLANE_HPP

#include "glm.hpp"

#include "ray.hpp"
#include "intersection.hpp"

namespace math
{
class Plane
{
public:
    Plane(glm::vec3 normal, glm::vec3 origin);

    bool intersect(math::Intersection & nearest,
                   const math::Ray & ray);

    glm::vec3 normal;
    glm::vec3 origin;
};
} // namespace math

#endif
