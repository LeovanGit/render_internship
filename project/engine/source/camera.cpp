#include "camera.hpp"

Camera::Camera(const glm::vec3 & position,
               const glm::vec3 & up,
               const glm::vec3 & forward)
{
    glm::vec3 right = glm::normalize(glm::cross(up, forward));

    // view_matrix_inv is camera model matrix!
    view_matrix_inv = glm::mat4(right.x,    right.y,    right.z,    0,
                                up.x,       up.y,       up.z,       0,
                                forward.x,  forward.y,  forward.z,  0,
                                position.x, position.y, position.z, 1.0f);

    rotation = glm::quat_cast(glm::mat3(view_matrix_inv));

    is_updated_basis = false;
    is_roll_enabled = false;
}

void Camera::setPerspective(float fovy,
                            float aspect,
                            float near,
                            float far)
{
    float p1 = 1 / tanf(fovy / 2);
    float p0 = p1 / aspect;

    // reversed depth
    proj_matrix = glm::mat4
        (p0, 0,  0,                            0,
         0,  p1, 0,                            0,
         0,  0,  near / (near - far),           1.0f,
         0,  0,  (-far * near) / (near - far), 0);

    proj_matrix_inv = glm::inverse(proj_matrix);

    is_updated_matrices = false;
}

glm::vec3 Camera::getPosition() const 
{
    return glm::vec3(view_matrix_inv[3][0],
                     view_matrix_inv[3][1],
                     view_matrix_inv[3][2]);
}

glm::vec3 Camera::getUp() const
{
    return glm::vec3(view_matrix_inv[1][0],
                     view_matrix_inv[1][1],
                     view_matrix_inv[1][2]);
}

glm::vec3 Camera::getForward() const
{
    return glm::vec3(view_matrix_inv[2][0],
                     view_matrix_inv[2][1],
                     view_matrix_inv[2][2]);
}

glm::vec3 Camera::getRight() const
{
    return glm::vec3(view_matrix_inv[0][0],
                     view_matrix_inv[0][1],
                     view_matrix_inv[0][2]);
}

const glm::mat4 & Camera::getViewProjInv() const
{
    return view_proj_matrix_inv;
}

const glm::mat4 & Camera::getViewProj() const
{
    return view_proj_matrix;
}

void Camera::setWorldPosition(const glm::vec3 & position)
{
    view_matrix_inv[3][0] = position.x;
    view_matrix_inv[3][1] = position.y;
    view_matrix_inv[3][2] = position.z;

    is_updated_matrices = false;
}

void Camera::addWorldPosition(const glm::vec3 & position)
{
    view_matrix_inv[3][0] += position.x;
    view_matrix_inv[3][1] += position.y;
    view_matrix_inv[3][2] += position.z;

    is_updated_matrices = false;
}

void Camera::setWorldAngles(const math::EulerAngles & angles)
{
    rotation = math::quatFromEuler(angles);
    //glm::normalize(rotation);

    is_updated_basis = false;
    is_updated_matrices = false;
}

void Camera::addWorldAngles(const math::EulerAngles & angles)
{
    rotation = math::quatFromEuler(angles) * rotation;
    // glm::normalize(rotation);

    is_updated_basis = false;
    is_updated_matrices = false;
}

void Camera::addRelativeAngles(const math::EulerAngles & angles)
{
    math::Basis basis(getRight(),
                      glm::vec3(0, 1.0f, 0),
                      glm::vec3(0, 0, 0));

    if (is_roll_enabled)
    {
        basis.y = getUp();
        basis.z = getForward();
    }

    rotation = math::quatFromEuler(angles, basis) * rotation;
    // glm::normalize(rotation);

    is_updated_basis = false;
    is_updated_matrices = false;
}

void Camera::updateBasis()
{
    if (is_updated_basis) return;

    glm::mat3 basis = glm::mat3_cast(rotation);
    
    for (int row = 0; row != 3; ++row)
    {
        for (int col = 0; col != 3; ++col)
        {
            view_matrix_inv[row][col] = basis[row][col];
        }
    }

    is_updated_basis = true;
}

void Camera::updateMatrices()
{
    if (is_updated_matrices) return;
    
    updateBasis();

    view_matrix = glm::inverse(view_matrix_inv);
    view_proj_matrix = proj_matrix * view_matrix;
    view_proj_matrix_inv = view_matrix_inv *
                           glm::inverse(proj_matrix);
    
    is_updated_matrices = true;
}

// generate WS point from CS
glm::vec3 Camera::reproject(float x, float y) const
{
    // z = 1.0f -> on near plane (reversed depth)
    glm::vec4 point_h_cs(x, y, 1.0f, 1.0f);
    glm::vec4 point_h_ws = view_proj_matrix_inv * point_h_cs;

    glm::vec3 point_ws = glm::vec3(point_h_ws) / point_h_ws.w;

    return point_ws;
}

std::vector<Camera> Camera::generateCubemapCameras(const glm::vec3 & position,
                                                   float near,
                                                   float far)
{
    std::vector<Camera> cameras =
    {
        // +x
        Camera(position,
               glm::vec3(0.0f, 1.0f, 0.0f),
               glm::vec3(1.0f, 0.0f, 0.0f)),
        // -x
        Camera(position,
               glm::vec3(0.0f, 1.0f, 0.0f),
               glm::vec3(-1.0f, 0.0f, 0.0f)),
        // +y
        Camera(position,
               glm::vec3(0.0f, 0.0f, -1.0f),
               glm::vec3(0.0f, 1.0f, 0.0f)),
        // -y
        Camera(position,
               glm::vec3(0.0f, 0.0f, 1.0f),
               glm::vec3(0.0f, -1.0f, 0.0f)),
        // +z
        Camera(position,
               glm::vec3(0.0f, 1.0f, 0.0f),
               glm::vec3(0.0f, 0.0f, 1.0f)),
        // -z
        Camera(position,
               glm::vec3(0.0f, 1.0f, 0.0f),
               glm::vec3(0.0f, 0.0f, -1.0f)),
    };

    for (uint32_t i = 0, size = cameras.size(); i != size; ++i)
    {
        cameras[i].setPerspective(glm::radians(90.0f),
                                  1.0f,
                                  near,
                                  far);
        cameras[i].updateMatrices();
    }

    return cameras;
}
