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

// Moller-Trumbore ray-triangle intersection
bool Ray::intersect(MeshIntersection & nearest,
                    const glm::vec3 & V1,
                    const glm::vec3 & V2,
                    const glm::vec3 & V3) const
{
    glm::vec3 edge_1 = V2 - V1;
    glm::vec3 edge_2 = V3 - V1;

    glm::vec3 vec_1 = glm::cross(direction, edge_2);
    float det = glm::dot(edge_1, vec_1);

    // ray || triangle
    if (areAlmostEqual(fabs(det), 0.0f, SOME_SMALL_NUMBER)) return false;
    
    float det_inv = 1.0f / det;

    glm::vec3 vec_2 = origin - V1;
    float u = glm::dot(vec_2, vec_1) * det_inv;
    if (u < 0.0f || u > 1.0f) return false;

    glm::vec3 vec_3 = glm::cross(vec_2, edge_1);
    float v = glm::dot(direction, vec_3) * det_inv;
    if (v < 0.0f || (u + v) > 1.0f) return false;

    float t = glm::dot(edge_2, vec_3) * det_inv;
    if (t < 0) return false; // no intersection
    if (t >= nearest.t) return false; // intersection, but not nearest
    
    nearest.t = t;
    nearest.pos = origin + t * direction;

    return true;
}

} // namespace math
