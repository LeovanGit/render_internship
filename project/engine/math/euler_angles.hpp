#ifndef EULER_ANGLES_HPP
#define EULER_ANGLES_HPP

#include "glm.hpp"
#include "gtc/quaternion.hpp"
#include <cmath>

namespace math
{
class EulerAngles
{
public:
    EulerAngles(float yaw,
                float pitch,
                float roll);

    float yaw;
    float pitch;
    float roll;
};

class Basis
{
public:
    Basis(const glm::vec3 & x,
          const glm::vec3 & y,
          const glm::vec3 & z);

    glm::vec3 x;
    glm::vec3 y;
    glm::vec3 z;
};

glm::quat quatFromEuler(const EulerAngles & angles,
                        const Basis & basis);

bool areAlmostEqual(float a, float b, float delta);
} // namespace math

#endif
