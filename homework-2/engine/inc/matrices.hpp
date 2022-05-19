#ifndef MATRICES_HPP
#define MATRICES_HPP

#include "glm.hpp"
#include "gtc/quaternion.hpp"

#include <cmath>

class Transform
{
public:
    Transform() = default;

    Transform(glm::vec3 position,
              glm::vec3 angles,
              glm::vec3 scale);

    glm::mat4 toMat4() const;

    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 position;
};

#endif
