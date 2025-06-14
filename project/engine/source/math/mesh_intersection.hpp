#ifndef MESH_INTERSECTION_H
#define MESH_INTERSECTION_H

#include "glm.hpp"
#include <limits>

#include "box.hpp"

namespace math
{
struct MeshIntersection
{
    glm::vec3 pos;
    float t;
    uint32_t triangle;
    uint32_t transform_id;
    uint16_t model_id;
    BoundingBox box;

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
