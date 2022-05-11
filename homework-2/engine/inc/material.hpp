#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "math.hpp"

class Material
{
public:
    Material() = default;

    Material(vec3 albedo, float specular, float glossiness, vec3 emission);

    vec3 albedo; // [0;1] material RGB color
    float specular; // [0;1]
    float glossiness; // [0;inf]
    vec3 emission; // [0; inf] own glow
};

#endif
