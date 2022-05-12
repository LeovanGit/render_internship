#ifndef RAY_HPP
#define RAY_HPP

#include "glm.hpp"

class Ray
{
public:
    Ray() = default;

    Ray(glm::vec3 origin, glm::vec3 direction);

glm::vec3 origin;
glm::vec3 direction;
};

#endif
