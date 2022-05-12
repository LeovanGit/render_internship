#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "glm.hpp"

class Material
{
public:
    Material() = default;

    Material(glm::vec3 albedo,
             float specular,
             float glossiness,
             glm::vec3 emission);

    glm::vec3 albedo; // [0;1] material RGB color
    float specular; // [0;1]
    float glossiness; // [0;inf]
    glm::vec3 emission; // [0; inf] own glow
};

#endif
