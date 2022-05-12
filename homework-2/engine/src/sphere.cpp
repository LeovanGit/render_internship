#include "sphere.hpp"

Sphere::Sphere(float radius, glm::vec3 origin) :
    radius(radius), origin(origin)
    {}

// return true only if Intersection was rewrited
// (current intersection is nearer than stored in Intersection)
bool Sphere::intersect(Intersection & nearest, Ray & ray)
{
    glm::vec3 ray_to_sphere_origin = ray.origin - origin;

    // quadratic equation coeff
    float a = 1; // dot(ray_direction, ray_direction)
    float b = 2.0f * glm::dot(ray.direction, ray_to_sphere_origin);
    float c = glm::dot(ray_to_sphere_origin, ray_to_sphere_origin) -
              radius * radius;

    float discriminant = b * b - 4 * a * c;
    if (discriminant < 0) return false; // no intersection

    float t_near = (-b - sqrt(discriminant)) / (2 * a);
    if (t_near >= nearest.t) return false; // intersection, but not nearest

    nearest.t = t_near;
    nearest.point = ray.origin + t_near * ray.direction;
    nearest.normal = glm::normalize(nearest.point - origin);

    return true;
}
