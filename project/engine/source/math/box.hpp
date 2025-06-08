#ifndef BOX_HPP
#define BOX_HPP

#include <limits>
#include "glm.hpp"

namespace math
{
struct BoundingBox
{
    glm::vec3 min;
    glm::vec3 max;

    static constexpr float inf = std::numeric_limits<float>::infinity();
    static constexpr BoundingBox empty() { return {{inf, inf, inf}, {-inf, -inf, -inf}}; }
    static constexpr BoundingBox unit() { return {{-1.f, -1.f, -1.f}, {1.f, 1.f, 1.f}}; }

    glm::vec3 size() const { return max - min; }
    glm::vec3 center() const { return (min + max) / 2.0f; }
    float diameter() const { return length(size()); }
    float radius() const { return diameter() / 2.0f; }

    void reset()
    {
        constexpr float maxf = std::numeric_limits<float>::max();
        min = { maxf , maxf , maxf };
        max = -min;
    }

    void expand(const BoundingBox & other)
    {
        min = glm::min(min, other.min);
        max = glm::max(max, other.max);
    }

    void expand(const glm::vec3 & point)
    {
        min = glm::min(min, point);
        max = glm::max(max, point);
    }

    bool contains(const glm::vec3 & P)
    {
        return min[0] <= P[0] && P[0] <= max[0] &&
               min[1] <= P[1] && P[1] <= max[1] &&
               min[2] <= P[2] && P[2] <= max[2];
    }
};
}

#endif
