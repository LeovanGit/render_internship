#include "ray.hpp"

namespace math
{
Ray::Ray(const glm::vec3 & origin,
               const glm::vec3 & direction) :
               origin(origin),
               direction(direction)
{}

// ray-box intersection
bool Ray::intersect(float t, const BoundingBox & box) const
{    
    glm::vec3 t1 = (box.min - origin) / direction;
    glm::vec3 t2 = (box.max - origin) / direction;

    if (t1.x > t2.x) std::swap(t1.x, t2.x);
    if (t1.y > t2.y) std::swap(t1.y, t2.y);
    if (t1.z > t2.z) std::swap(t1.z, t2.z);

    float t_near = fmax(fmax(t1.x, t1.y), t1.z);
    float t_far = fmin(fmin(t2.x, t2.y), t2.z);

    if (t_far < 0 || t_near > t_far) return false;
    if (t_near >= t) return false;
    
    return true;
}

// ray-triangle intersection
bool Ray::intersect(MeshIntersection & nearest,
                    const glm::vec3 & V1,
                    const glm::vec3 & V2,
                    const glm::vec3 & V3) const
{
    glm::vec3 normal = glm::normalize(glm::cross(V2 - V1, V3 - V1));
    
    float cosa = glm::dot(normal, direction);
    // ray || triangle
    if (math::areAlmostEqual(cosa, 0, math::SOME_SMALL_NUMBER)) return false; 
    
    float d = -glm::dot(normal, V1);
    float t = -(d + glm::dot(normal, origin)) / cosa;

    if (t < 0) return false; // no intersection
    if (t >= nearest.t) return false; // intersection, but not nearest

    // inside-outside the triangle test
    glm::vec3 point = origin + direction * t;
    
    glm::vec3 edge_1 = V2 - V1;
    glm::vec3 edge_2 = V3 - V2;
    glm::vec3 edge_3 = V1 - V3;

    glm::vec3 vp_1 = point - V1;
    glm::vec3 vp_2 = point - V2;
    glm::vec3 vp_3 = point - V3;

    glm::vec3 n = normal;
    if (glm::dot(glm::cross(edge_1, edge_2), normal) <= 0) n = -normal;

    if (glm::dot(n, glm::cross(edge_1, vp_1)) <= 0 ||
        glm::dot(n, glm::cross(edge_2, vp_2)) <= 0 ||
        glm::dot(n, glm::cross(edge_3, vp_3)) <= 0) return false;
    
    nearest.t = t;
    nearest.pos = origin + t * direction;

    return true;
}

} // namespace math
