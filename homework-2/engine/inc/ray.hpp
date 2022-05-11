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

// ray should go from intersection point, not into!
inline vec3 reflect(vec3 ray, vec3 normal)
{
    return normal * dot(normal, ray) * 2 - ray;
}

#endif
