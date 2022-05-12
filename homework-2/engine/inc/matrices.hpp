#ifndef MATRICES_HPP
#define MATRICES_HPP

#include "glm.hpp"
#include <cmath>

class Transform
{
public:
    Transform() = default;

    Transform(glm::vec3 position,
              glm::vec3 rotation,
              glm::vec3 scale);

    glm::mat4 toMatrix() const;

    glm::vec3 scale;
    glm::vec3 rotation;
    glm::vec3 position;
};

glm::mat4 perspective(float fovy,
                      float aspect,
                      float near,
                      float far);

#endif
