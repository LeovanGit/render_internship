#include "camera.hpp"

Camera::Camera(glm::vec3 position,
               glm::vec3 up,
               glm::vec3 forward)
{
    glm::vec3 right = glm::normalize(glm::cross(forward, up));

    // world center in view space
    glm::vec3 worldVS(-glm::dot(position, right),
                      -glm::dot(position, up),
                      -glm::dot(position, forward));

    view_matrix = glm::mat4(right.x, up.x, forward.x, 0,
                            right.y, up.y, forward.y, 0,
                            right.z, up.z, forward.z, 0,
                            worldVS.x, worldVS.y, worldVS.z, 1.0f);

    view_matrix_inv = glm::inverse(view_matrix);
}

void Camera::setPerspective(float fovy,
                            float aspect,
                            float near,
                            float far)
{
    float p1 = 1 / tan(fovy / 2);
    float p0 = p1 / aspect;

    // reversed depth!
    proj_matrix = glm::mat4
        (p0, 0,  0,                            0,
         0,  p1, 0,                            0,
         0,  0,  near / (near - far),          1.0f,
         0,  0,  (-far * near) / (near - far), 0);

    proj_matrix_inv = glm::inverse(proj_matrix);

    view_proj_matrix = view_matrix * proj_matrix;

    view_proj_matrix_inv = glm::inverse(view_matrix) *
                           glm::inverse(proj_matrix);
}

glm::vec3 Camera::getPosition() const
{
    return glm::vec3(view_matrix_inv[3][0],
                     view_matrix_inv[3][1],
                     view_matrix_inv[3][2]);
}

glm::vec3 Camera::getUp() const
{
    return glm::vec3(view_matrix[0][1],
                     view_matrix[1][1],
                     view_matrix[2][1]);
}

glm::vec3 Camera::getForward() const
{
    return glm::vec3(view_matrix[0][2],
                     view_matrix[1][2],
                     view_matrix[2][2]);
}

glm::vec3 Camera::getRight() const
{
    return glm::vec3(view_matrix[0][0],
                     view_matrix[1][0],
                     view_matrix[2][0]);
}

const glm::mat4 & Camera::getViewProjInv() const
{
    return view_proj_matrix_inv;
}
