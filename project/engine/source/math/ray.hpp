#ifndef RAY_HPP
#define RAY_HPP

#include "glm.hpp"

#include "box.hpp"
#include "euler_angles.hpp"
#include "constants.hpp"
#include "mesh_intersection.hpp"

namespace math
{
class Ray
{
public:
    Ray() = default;

    Ray(const glm::vec3 & origin,
        const glm::vec3 & direction);

    bool intersect(float t, const BoundingBox & box) const;

    bool intersect(MeshIntersection & nearest,
                   const glm::vec3 & V1,
                   const glm::vec3 & V2,
                   const glm::vec3 & V3) const;

glm::vec3 origin;
glm::vec3 direction;
};
} // namespace math

#endif

