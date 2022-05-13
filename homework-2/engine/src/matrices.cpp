#include "matrices.hpp"

#include <iostream>
Transform::Transform(glm::vec3 position,
                     glm::vec3 rotation,
                     glm::vec3 scale) :
                     position(position),
                     rotation(rotation),
                     scale(scale)
{}

glm::mat4 Transform::toMatrix() const
{
    // TRANSLATION
    glm::mat4 translation(1.0f,       0,          0,          0,
                          0,          1.0f,       0,          0,
                          0,          0,          1.0f,       0,
                          position.x, position.y, position.z, 1.0f);

    // ROTATION
    glm::mat4 rotation_x(1.0f, 0,               0,                0,
                         0,    cos(rotation.x), -sin(rotation.x), 0,
                         0,    sin(rotation.x), cos(rotation.x),  0,
                         0,    0,               0,                1.0f);

    glm::mat4 rotation_y(cos(rotation.y),  0,    sin(rotation.y), 0,
                         0,                1.0f, 0,               0,
                         -sin(rotation.y), 0,    cos(rotation.y), 0,
                         0,                0,    0,               1.0f);
    
    glm::mat4 rotation_z(cos(rotation.z), -sin(rotation.z), 0,    0,
                         sin(rotation.z), cos(rotation.z),  0,    0,
                         0,               0,                1.0f, 0,
                         0,               0,                0,    1.0f);

    // z -> x -> y
    glm::mat4 rotation = rotation_y * rotation_x * rotation_z;

    // SCALING
    glm::mat4 scaling(scale.x, 0,       0,       0,
                      0,       scale.y, 0,       0,
                      0,       0,       scale.z, 0,
                      0,       0,       0,       1.0f);

    // scale -> rotate -> translate
    return translation * rotation * scaling;
}
