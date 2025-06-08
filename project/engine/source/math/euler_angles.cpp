#include "euler_angles.hpp"

namespace math
{
EulerAngles::EulerAngles(float yaw,
                         float pitch,
                         float roll) :
                         yaw(yaw),
                         pitch(pitch),
                         roll(roll) 
{}

Basis::Basis(const glm::vec3 & x,
             const glm::vec3 & y,
             const glm::vec3 & z) :
             x(x),
             y(y),
             z(z)
{}

glm::quat quatFromEuler(const EulerAngles & angles,
                        const Basis & basis)
{
    // this is Z then X then Y
    // just multiplication should be written in reverse order
    // because of glm is column major
    glm::quat q = glm::angleAxis(glm::radians(angles.yaw), basis.y);
    q *= glm::angleAxis(glm::radians(angles.pitch), basis.x);
    q *= glm::angleAxis(glm::radians(angles.roll), basis.z);

    return q;
}

bool areAlmostEqual(float a, float b, float epsilon)
{
    return std::fabs(a - b) < epsilon;
}

glm::mat4 rotateZ(float angle)
{
    float cosa = std::cosf(angle);
    float sina = std::sinf(angle);

    glm::mat4x4 rotate(cosa, -sina, 0.0f, 0.0f,
                       sina,  cosa, 0.0f, 0.0f,
                       0.0f,  0.0f, 1.0f, 0.0f,
                       0.0f,  0.0f, 0.0f, 1.0f);        
    
    return rotate;
}
} // namespace math
