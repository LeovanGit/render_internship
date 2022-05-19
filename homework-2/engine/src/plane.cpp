#include "plane.hpp"

Plane::Plane(glm::vec3 normal, glm::vec3 origin) :
             normal(normal), origin(origin)
{}

bool Plane::intersect(Intersection & nearest, Ray & ray)
{
    // ray || plane
    float d = dot(normal, ray.direction);
    if (d == 0) return false;
    
    float t = glm::dot((origin - ray.origin), normal) / d;

    if (t < 0) return false; // no intersection
    if (t >= nearest.t) return false; // intersection, but not nearest

    nearest.t = t;
    nearest.point = ray.origin + ray.direction * t;
    nearest.normal = normal;

    return true;
}
