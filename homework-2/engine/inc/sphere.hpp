#ifndef SPHERE_HPP
#define SPHERE_HPP

#include <cmath>

#include "math.hpp"
#include "ray.hpp"
#include "intersection.hpp"

class Sphere
{
public:
    Sphere(float radius, vec3 origin);

    Sphere(float radius, float x, float y, float z);

    float radius;
    vec3 origin;
};

#endif
