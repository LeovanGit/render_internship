#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "math.hpp"

class Sphere
{
public:
    Sphere(float radius, vec3 origin);

    Sphere(float radius, float x, float y, float z);

    bool isIntersect(vec3 ray_origin, vec3 ray_direction);

    float radius;
    vec3 origin;
};

#endif
