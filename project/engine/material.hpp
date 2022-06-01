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
             const glm::vec3 & fresnel,
             const glm::vec3 & emission);

    glm::vec3 albedo;
    float specular;
    float glossiness;
    glm::vec3 fresnel;
    glm::vec3 emission;
};

#endif
