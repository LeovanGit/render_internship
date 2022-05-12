#include "plane.hpp"

Plane::Plane(glm::vec3 a, glm::vec3 b, glm::vec3 c) :
             a(a), b(b), c(c)
{
    glm::vec3 ab = a - b;
    glm::vec3 ac = a - c;
    normal = glm::normalize(glm::cross(ab, ac));
}

bool Plane::intersect(Intersection & nearest, Ray & ray)
{
    float d = -dot(normal, a);   
    float t = -(dot(normal, ray.origin) + d) / dot(normal, ray.direction);

    if (t >= nearest.t) return false;

    nearest.t = t;
    nearest.point = ray.origin + ray.direction * t;
    nearest.normal = normal;
    nearest.material = Material(glm::vec3(0.8f, 0.0f, 0.0f),
                                0,
                                0,
                                glm::vec3(0, 0, 0));
    nearest.type = PLANE;

    return true;
}
