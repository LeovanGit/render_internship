#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "glm.hpp"

class Material
{
public:
    Material() = default;

    Material(const glm::vec3 & albedo,
             float specular,
             float glossiness,
             float metalness,
             const glm::vec3 & emission);

    glm::vec3 albedo;
    float specular;
    float glossiness;
    float metalness;
    glm::vec3 emission;
};

#endif
