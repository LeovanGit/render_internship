#include "sphere.hpp"

math::Sphere::Sphere(float radius, glm::vec3 origin) :
    radius(radius), origin(origin)
    {}

// return true only if Intersection was rewrited
// (because current intersection is nearer than stored in Intersection)
bool math::Sphere::intersect(Intersection & nearest, const Ray & ray)
{
    // L - vector from ray origin to sphere origin
    glm::vec3 L = ray.origin - origin;

    // quadratic equation coeff
    float a = 1; // dot(ray.direction, ray.direction)
    float b = 2.0f * glm::dot(ray.direction, L);
    float c = glm::dot(L, L) - radius * radius;

    float discriminant = b * b - 4 * a * c;
    if (discriminant < 0) return false; // no intersection

    float t_near = (-b - sqrt(discriminant)) / (2 * a);
    if (t_near <= 0) return false; // no intersection
    if (t_near >= nearest.t) return false; // intersection, but not nearest

    nearest.t = t_near;
    nearest.point = ray.origin + t_near * ray.direction;
    nearest.normal = glm::normalize(nearest.point - origin);

    return true;
}
