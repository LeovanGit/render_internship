#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "glm.hpp"

#include "ray.hpp"
#include "intersection.hpp"

namespace math
{
class Sphere
{
public:
    Sphere(float radius, const glm::vec3 & origin);

    bool intersect(Intersection & nearest, const Ray & ray);

    float radius;
    glm::vec3 origin;
};
} // namespace math

#endif
