#include "triangle.hpp"

math::Triangle::Triangle(glm::vec3 normal,
                         glm::vec3 vertex_1,
                         glm::vec3 vertex_2,
                         glm::vec3 vertex_3) :
                         normal(normal),
                         vertex_1(vertex_1),
                         vertex_2(vertex_2),
                         vertex_3(vertex_3)
{}

bool math::Triangle::intersect(Intersection & nearest,
                               const Ray & ray) const
{
    float cosa = glm::dot(normal, ray.direction);
    if (cosa == 0) return false; // ray || triangle
    
    float d = -glm::dot(normal, vertex_1);
    float t = -(d + glm::dot(normal, ray.origin)) / cosa;

    if (t < 0) return false; // no intersection
    if (t >= nearest.t) return false; // intersection, but not nearest
    
    // inside-outside the triangle test
    glm::vec3 point = ray.origin + ray.direction * t;
    
    glm::vec3 edge_1 = vertex_2 - vertex_1;
    glm::vec3 edge_2 = vertex_3 - vertex_2;
    glm::vec3 edge_3 = vertex_1 - vertex_3;

    glm::vec3 vp_1 = point - vertex_1;
    glm::vec3 vp_2 = point - vertex_2;
    glm::vec3 vp_3 = point - vertex_3;

    glm::vec3 n = normal;
    if (glm::dot(glm::cross(edge_1, edge_2), normal) <= 0) n = -normal;

    if (glm::dot(n, glm::cross(edge_1, vp_1)) <= 0 ||
        glm::dot(n, glm::cross(edge_2, vp_2)) <= 0 ||
        glm::dot(n, glm::cross(edge_3, vp_3)) <= 0) return false;

    nearest.t = t;
    nearest.point = point;
    nearest.normal = normal;

    return true;
}
