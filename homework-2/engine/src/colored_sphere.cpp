#include "colored_sphere.hpp"

ColoredSphere::ColoredSphere(Sphere sphere, Material material) : 
                             sphere(sphere), material(material) {}

bool ColoredSphere::intersect(Intersection & nearest, Ray & ray)
{
    if (sphere.intersect(nearest, ray))
    {
        nearest.material = material;
        nearest.type = SPHERE;

        return true;
    }

    return false;
}


