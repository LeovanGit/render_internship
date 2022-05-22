#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "glm.hpp"
#include "gtc/quaternion.hpp"

#include "matrices.hpp"

#include <iostream>

class Camera
{
public:
    Camera(glm::vec3 position,
           glm::vec3 up,
           glm::vec3 forward);

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

    void setWorldPosition(const glm::vec3 & position);

    void addWorldPosition(const glm::vec3 & position);

    void setWorldAngles(const glm::vec3 & angles);

    void addWorldAngles(const glm::vec3 & angles);

    void addRelativeAngles(const glm::vec3 & angles);

    void updateBasis();

    void updateMatrices();

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
