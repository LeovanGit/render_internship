#include "matrices.hpp"
#include "gtc/quaternion.hpp"
#include "matrix.hpp"

math::Transform::Transform(glm::vec3 position,
                           glm::vec3 angles,
                           glm::vec3 scale) :
                           position(position),
                           scale(scale)                         
{
    // oz
    rotation = glm::quat(cos(glm::radians(angles.z / 2)),
                         (float)sin(glm::radians(angles.z / 2)) *
                         glm::vec3(0, 0, 1.0f));

    // ox
    rotation *= glm::quat(cos(glm::radians(angles.x / 2)),
                          (float)sin(glm::radians(angles.x / 2)) *
                          glm::vec3(1.0f, 0, 0));

    // oy
    rotation *= glm::quat(cos(glm::radians(angles.y / 2)),
                          (float)sin(glm::radians(angles.y / 2)) *
                          glm::vec3(0, 1.0f, 0));    
}

glm::mat4 math::Transform::toMat4() const
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
