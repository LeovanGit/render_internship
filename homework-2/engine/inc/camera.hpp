#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "glm.hpp"

class Camera
{
public:
    Camera() = default;

    Camera(glm::vec3 position,
           glm::vec3 up,
           glm::vec3 forward);

    glm::mat4 getViewMatrix() const;

glm::vec3 position;
glm::vec3 up;
glm::vec3 forward;
};

#endif
