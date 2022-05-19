#ifndef COLORED_TRIANGLE_HPP
#define COLORED_TRIANGLE_HPP

#include "triangle.hpp"
#include "material.hpp"

class ColoredTriangle
{
public:
    ColoredTriangle (Triangle triangle, Material material);

    bool intersect(Intersection & nearest, Ray & ray);

Triangle triangle;
Material material;
};

#endif
