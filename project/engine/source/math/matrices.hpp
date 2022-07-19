#ifndef MATRICES_HPP
#define MATRICES_HPP

#include "glm.hpp"
#include "gtc/quaternion.hpp"
#include "matrix.hpp"
#include <cmath>

#include "euler_angles.hpp"

namespace math
{
class Transform
{
public:
    Transform() = default;

    Transform(const glm::vec3 & position,
              const EulerAngles & angles,
              const glm::vec3 & scale);

    glm::mat4 toMat4() const;

    glm::vec3 position;
    glm::vec3 scale;
    glm::quat rotation;
};
} // namespace math

#endif
