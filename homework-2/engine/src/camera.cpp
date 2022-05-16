#include "camera.hpp"
#include "gtc/quaternion.hpp"

Camera::Camera(glm::vec3 position,
               glm::vec3 up,
               glm::vec3 forward)
{
    glm::vec3 right = glm::normalize(glm::cross(up, forward));

    // view_matrix_inv is camera model matrix!
    view_matrix_inv = glm::mat4(right.x,    right.y,    right.z,    0,
                                up.x,       up.y,       up.z,       0,
                                forward.x,  forward.y,  forward.z,  0,
                                position.x, position.y, position.z, 1.0f);

    rotation = glm::quat_cast(glm::mat3(right, up, forward));
    is_updated_basis = false;
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

    is_updated_matrices = false;
}

glm::vec3 Camera::getPosition() const { return view_matrix_inv[3]; }

glm::vec3 Camera::getUp() const { return view_matrix_inv[1]; }

glm::vec3 Camera::getForward() const { return view_matrix_inv[2]; }

glm::vec3 Camera::getRight() const { return view_matrix_inv[0]; }

const glm::mat4 & Camera::getViewProjInv() const
{
    return view_proj_matrix_inv;
}

void Camera::setPosition(const glm::vec3 & position)
{
    view_matrix_inv[3][0] = position.x;
    view_matrix_inv[3][1] = position.y;
    view_matrix_inv[3][2] = position.z;

    is_updated_matrices = false;
}

void Camera::addPosition(const glm::vec3 & position)
{
    view_matrix_inv[3][0] += position.x;
    view_matrix_inv[3][1] += position.y;
    view_matrix_inv[3][2] += position.z;

    is_updated_matrices = false;
}

void Camera::setAngles(const glm::vec3 & angles)
{
    // ox
    rotation = glm::quat(cos(glm::radians(angles.x / 2)),
                         (float)sin(glm::radians(angles.x / 2)) *
                         glm::vec3(1.0f, 0, 0));

    // oy
    rotation *= glm::quat(cos(glm::radians(angles.y / 2)),
                          (float)sin(glm::radians(angles.y / 2)) *
                          glm::vec3(0, 1.0f, 0));

    // oz
    rotation *= glm::quat(cos(glm::radians(angles.z / 2)),
                          (float)sin(glm::radians(angles.z / 2)) *
                          glm::vec3(0, 0, 1.0f));

    //glm::normalize(rotation);

    is_updated_basis = false;
    is_updated_matrices = false;
}

void Camera::addAngles(const glm::vec3 & angles)
{
    // ox
    rotation *= glm::quat(cos(glm::radians(angles.x / 2)),
                          (float)sin(glm::radians(angles.x / 2)) *
                          glm::vec3(1.0f, 0, 0));

    // oy
    rotation *= glm::quat(cos(glm::radians(angles.y / 2)),
                          (float)sin(glm::radians(angles.y / 2)) *
                          glm::vec3(0, 1.0f, 0));

    // oz
    rotation *= glm::quat(cos(glm::radians(angles.z / 2)),
                          (float)sin(glm::radians(angles.z / 2)) *
                          glm::vec3(0, 0, 1.0f));

    //glm::normalize(rotation);

    is_updated_basis = false;
    is_updated_matrices = false;
}

void Camera::updateBasis()
{
    if (is_updated_basis) return;

    glm::mat3 basis = glm::mat3_cast(rotation);
    
    for (int i = 0; i != 3; ++i)
    {
        for (int j = 0; j!= 3; ++j)
        {
            view_matrix_inv[i][j] = basis[i][j];
        }
    }    

    is_updated_basis = true;
}

void Camera::updateMatrices()
{
    if (is_updated_matrices) return;
    
    updateBasis();

    view_matrix = glm::inverse(view_matrix_inv);
    view_proj_matrix = view_matrix * proj_matrix;
    view_proj_matrix_inv = glm::inverse(view_matrix) *
                           glm::inverse(proj_matrix);
    
    is_updated_matrices = true;
}
