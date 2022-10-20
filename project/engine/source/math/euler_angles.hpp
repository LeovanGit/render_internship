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

    EulerAngles operator-() { return EulerAngles(-yaw, -pitch, -roll); };
    
    float yaw;
    float pitch;
    float roll;
};

class Basis
{
public:
    Basis(const glm::vec3 & x = glm::vec3(1.0f, 0.0f, 0.0f),
          const glm::vec3 & y = glm::vec3(0.0f, 1.0f, 0.0f),
          const glm::vec3 & z = glm::vec3(0.0f, 0.0f, 1.0f));

    glm::vec3 x;
    glm::vec3 y;
    glm::vec3 z;
};

glm::quat quatFromEuler(const EulerAngles & angles,
                        const Basis & basis = Basis(glm::vec3(1.0f, 0, 0),
                                                    glm::vec3(0, 1.0f, 0),
                                                    glm::vec3(0, 0, 1.0f)));

bool areAlmostEqual(float a, float b, float epsilon);
} // namespace math

#endif
