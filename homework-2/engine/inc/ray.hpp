#ifndef RAY_HPP
#define RAY_HPP

#include "math.hpp"

class Ray
{
public:
    Ray() = default;

    Ray(vec3 origin, vec3 direction);

vec3 origin;
vec3 direction;
};

#endif
