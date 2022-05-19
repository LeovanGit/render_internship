#include "colored_triangle.hpp"

ColoredTriangle::ColoredTriangle(Triangle triangle, Material material) : 
    triangle(triangle), material(material) {}

bool ColoredTriangle::intersect(Intersection & nearest, Ray & ray)
{
    if (triangle.intersect(nearest, ray))
    {
        nearest.material = material;
        nearest.type = TRIANGLE;

        return true;
    }

    return false;
}


