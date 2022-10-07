#include "matrices.hpp"
#include "euler_angles.hpp"

namespace math
{
Transform::Transform(const glm::vec3 & position,
                     const EulerAngles & angles,
                     const glm::vec3 & scale) :
                     position(position),
                     rotation(quatFromEuler(angles, Basis())),
                     scale(scale)
{}

glm::mat4 Transform::toMat4() const
{
    // glm is column-major!!!
    glm::mat4 translation(1.0f,       0,          0,          0,
                          0,          1.0f,       0,          0,
                          0,          0,          1.0f,       0,
                          position.x, position.y, position.z, 1.0f);

    glm::mat4 scaling(scale.x, 0,       0,       0,
                      0,       scale.y, 0,       0,
                      0,       0,       scale.z, 0,
                      0,       0,       0,       1.0f);

    // scale -> rotate -> translate
    return translation * glm::mat4_cast(rotation) * scaling;
}
} // namespace math
