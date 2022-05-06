#include "sphere.hpp"

Sphere::Sphere(float radius, vec3 origin) :
    radius(radius), origin(origin)
    {}

Sphere::Sphere(float radius, float x, float y, float z) :
    radius(radius), origin(vec3(x, y, z))
    {}

bool Sphere::isIntersect(vec3 ray_origin, vec3 ray_direction)
{
    vec3 ray_to_sphere_origin = ray_origin - origin;

    // quadratic equation coeff
    float a = 1; // dot(ray_direction, ray_direction)
    float b = 2.0f * dot(ray_direction, ray_to_sphere_origin);
    float c = dot(ray_to_sphere_origin, ray_to_sphere_origin) -
              radius * radius;

    float discriminant = b * b - 4 * a * c;

    return discriminant >= 0;
}

