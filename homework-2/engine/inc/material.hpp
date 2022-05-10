#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "math.hpp"

class Material
{
public:
    Material() = default;

    Material(vec3 color);

    vec3 color;
};

#endif
