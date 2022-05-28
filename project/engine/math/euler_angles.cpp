#include "euler_angles.hpp"

math::EulerAngles::EulerAngles(float yaw,
                               float pitch,
                               float roll) :
                               yaw(yaw),
                               pitch(pitch),
                               roll(roll) 
{}

math::Basis::Basis(const glm::vec3 & x,
                   const glm::vec3 & y,
                   const glm::vec3 & z) :
                   x(x),
                   y(y),
                   z(z)
{}

glm::quat math::quatFromEuler(const EulerAngles & angles,
                              const Basis & basis)
{
    glm::quat q = glm::angleAxis(glm::radians(angles.roll), basis.z);
    q *= glm::angleAxis(glm::radians(angles.pitch), basis.x);
    q *= glm::angleAxis(glm::radians(angles.yaw), basis.y);

    return q;
}

bool math::areAlmostEqual(float a, float b, float delta)
{
    return std::fabs(a - b) < delta;
}
