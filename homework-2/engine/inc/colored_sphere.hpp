#ifndef COLORED_SPHERE_HPP
#define COLORED_SPHERE_HPP

#include "sphere.hpp"
#include "material.hpp"

class ColoredSphere
{
public:
    ColoredSphere(Sphere sphere, Material material);

    bool intersect(Intersection & nearest, Ray & ray);

Sphere sphere;
Material material;
};

#endif
