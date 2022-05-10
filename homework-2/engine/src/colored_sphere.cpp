#include "colored_sphere.hpp"

ColoredSphere::ColoredSphere(Sphere sphere, Material material) : 
                             sphere(sphere), material(material) {}

// return true only if Intersection was rewrited
// (current intersection is nearer than stored in Intersection)
bool ColoredSphere::intersect(Intersection & nearest, Ray & ray)
{
    vec3 ray_to_sphere_origin = ray.origin - sphere.origin;

    // quadratic equation coeff
    float a = 1; // dot(ray_direction, ray_direction)
    float b = 2.0f * dot(ray.direction, ray_to_sphere_origin);
    float c = dot(ray_to_sphere_origin, ray_to_sphere_origin) -
              sphere.radius * sphere.radius;

    float discriminant = b * b - 4 * a * c;
    if (discriminant < 0) return false; // no intersection

    float t_near = (-b - sqrt(discriminant)) / (2 * a);
    if (t_near >= nearest.t) return false; // intersection, but not nearest

    nearest.t = t_near;
    nearest.point = ray.origin + ray.direction * t_near;
    nearest.material = material;

    return true;
}


