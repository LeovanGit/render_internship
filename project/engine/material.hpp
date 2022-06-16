#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "glm.hpp"

class Material
{
public:
    Material() = default;

    Material(const glm::vec3 & albedo,
             float roughness,
             float metalness,
             const glm::vec3 & emission,
             const glm::vec3 & fresnel);

    glm::vec3 albedo;
    float roughness;
    float metalness;
    glm::vec3 emission;
    glm::vec3 fresnel;
};

#endif
