#include "colored_plane.hpp"

ColoredPlane::ColoredPlane(Plane plane, Material material) : 
                           plane(plane), material(material) {}

bool ColoredPlane::intersect(Intersection & nearest, Ray & ray)
{
    if (plane.intersect(nearest, ray))
    {
        nearest.material = material;
        nearest.type = PLANE;

        return true;
    }

    return false;
}


