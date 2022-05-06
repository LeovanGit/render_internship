#include "math.hpp"

float dot(const vec3 & v1, const vec3 & v2)
{
    return v1.getX() * v2.getX() +
           v1.getY() * v2.getY() +
           v1.getZ() * v2.getZ();
}

