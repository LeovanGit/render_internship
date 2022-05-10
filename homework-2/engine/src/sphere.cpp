#include "sphere.hpp"

Sphere::Sphere(float radius, vec3 origin) :
    radius(radius), origin(origin)
    {}

Sphere::Sphere(float radius, float x, float y, float z) :
    radius(radius), origin(vec3(x, y, z))
    {}

