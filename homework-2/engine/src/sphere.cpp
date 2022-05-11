#include "sphere.hpp"

Sphere::Sphere(float radius, vec3 origin) :
    radius(radius), origin(origin)
    {}

Sphere::Sphere(float radius, float x, float y, float z) :
    radius(radius), origin(vec3(x, y, z))
    {}

// return true only if Intersection was rewrited
// (current intersection is nearer than stored in Intersection)
bool Sphere::intersect(Intersection & nearest, Ray & ray)
{
    vec3 ray_to_sphere_origin = ray.origin - origin;

    // quadratic equation coeff
    float a = 1; // dot(ray_direction, ray_direction)
    float b = 2.0f * dot(ray.direction, ray_to_sphere_origin);
    float c = dot(ray_to_sphere_origin, ray_to_sphere_origin) -
              radius * radius;

    float discriminant = b * b - 4 * a * c;
    if (discriminant < 0) return false; // no intersection

    float t_near = (-b - sqrt(discriminant)) / (2 * a);
    if (t_near >= nearest.t) return false; // intersection, but not nearest

    nearest.t = t_near;
    nearest.point = ray.origin + ray.direction * t_near;
    nearest.normal = (nearest.point - origin).getNormalized();

    return true;
}
