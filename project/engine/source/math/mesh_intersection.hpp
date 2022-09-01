#ifndef MESH_INTERSECTION_H
#define MESH_INTERSECTION_H

#include "glm.hpp"
#include <limits>

namespace math
{
struct MeshIntersection
{
    glm::vec3 pos;
    // float near;
    float t;
    uint32_t triangle;
    uint32_t transform_id;

    constexpr void reset(float near,
                         float far = std::numeric_limits<float>::infinity())
    {
        // this->near = near;
        t = far;
    }
    
    bool valid() const { return std::isfinite(t); }
};
} // namespace math

#endif /* MESH_INTERSECTION_H */
