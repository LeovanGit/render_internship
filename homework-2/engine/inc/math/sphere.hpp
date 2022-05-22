#ifndef SPHERE_HPP
#define SPHERE_HPP

#include <cmath>
#include <iostream>

#include "glm.hpp"
#include "ray.hpp"
#include "intersection.hpp"

namespace math
{
class Sphere
{
public:
    Sphere(float radius, glm::vec3 origin);

    bool intersect(Intersection & nearest, const Ray & ray);

    float radius;
    glm::vec3 origin;
};
} // namespace math

#endif
