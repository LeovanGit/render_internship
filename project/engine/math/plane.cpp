#include "plane.hpp"
#include "euler_angles.hpp"

math::Plane::Plane(glm::vec3 normal, glm::vec3 origin) :
             normal(normal), origin(origin)
{}

bool math::Plane::intersect(math::Intersection & nearest,
                            const math::Ray & ray)
{
    // ray || plane
    float d = dot(normal, ray.direction);
    if (math::areAlmostEqual(d, 0, 0.01f)) return false;
    
    float t = glm::dot((origin - ray.origin), normal) / d;

    if (t < 0) return false; // no intersection
    if (t >= nearest.t) return false; // intersection, but not nearest

    nearest.t = t;
    nearest.point = ray.origin + ray.direction * t;
    nearest.normal = normal;

    return true;
}
