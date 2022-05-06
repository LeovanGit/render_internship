#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "math.hpp"

class Sphere
{
public:
    Sphere(float radius, vec3 origin);

    explicit Sphere(float radius = 0, float x = 0, float y = 0, float z = 0);

    bool isIntersect(vec3 ray_origin, vec3 ray_direction);

    float radius;
    vec3 origin;
};

#endif
