#ifndef CAMERA_HPP
#define CAMERA_HPP
#include <iostream>
#include <vector>
#include "glm.hpp"
#include "gtc/quaternion.hpp"

#include "matrices.hpp"
#include "euler_angles.hpp"

class Camera
{
public:
    Camera(const glm::vec3 & position,
           const glm::vec3 & up,
           const glm::vec3 & forward);

    void setPerspective(float fovy,
                        float aspect,
                        float near,
                        float far);

    glm::vec3 getPosition() const;

    glm::vec3 getUp() const;

    glm::vec3 getForward() const;

    glm::vec3 getRight() const;

    const glm::mat4 & getViewProjInv() const;

    const glm::mat4 & getViewProj() const;

    const glm::mat4 & getView() const;

    const glm::mat4 & getViewInv() const;

    const glm::mat4 & getProj() const;

    void setWorldPosition(const glm::vec3 & position);

    void addWorldPosition(const glm::vec3 & position);

    void setWorldAngles(const math::EulerAngles & angles);

    void addWorldAngles(const math::EulerAngles & angles);

    void addRelativeAngles(const math::EulerAngles & angles);

    void updateBasis();

    void updateMatrices();

    glm::vec3 reproject(float x, float y) const;

    static std::vector<Camera> generateCubemapCameras(const glm::vec3 & position,
                                                      float near = 0.1f,
                                                      float far = 1000.0f);

    bool is_roll_enabled;

private:
    glm::mat4 view_matrix;
    glm::mat4 proj_matrix;
    glm::mat4 view_proj_matrix;

    glm::mat4 view_matrix_inv;
    glm::mat4 proj_matrix_inv;
    glm::mat4 view_proj_matrix_inv;

    glm::quat rotation;

    bool is_updated_basis;
    bool is_updated_matrices;
};

#endif
