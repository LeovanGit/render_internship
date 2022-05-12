#include "camera.hpp"

Camera::Camera(glm::vec3 position,
               glm::vec3 up,
               glm::vec3 forward) :
               position(position),
               up(glm::normalize(up)),
               forward(glm::normalize(forward))
{}

glm::mat4 Camera::getViewMatrix() const
{
    glm::vec3 right = glm::cross(forward, up);

    // world center in view space
    glm::vec3 worldVS(-glm::dot(position, right),
                      -glm::dot(position, up),
                      -glm::dot(position, forward));

    return glm::mat4(right.x, up.x, forward.x, 0,
                     right.y, up.y, forward.y, 0,
                     right.z, up.z, forward.z, 0,
                     worldVS.x, worldVS.y, worldVS.z, 1.0f);
}
