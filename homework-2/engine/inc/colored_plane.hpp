#ifndef COLORED_PLANE_HPP
#define COLORED_PLANE_HPP

#include "plane.hpp"
#include "material.hpp"

class ColoredPlane
{
public:
    ColoredPlane(Plane plane, Material material);

    bool intersect(Intersection & nearest, Ray & ray);

Plane plane;
Material material;
};

#endif
