#include "light.hpp"

PointLight::PointLight(glm::vec3 color,
                       float radius,
                       Sphere sphere,
                       Material material) :
                       color(color),
                       radius(radius),
                       sphere(sphere),
                       material(material)
{}

bool PointLight::intersect(Intersection & nearest, Ray & ray)
{
    if (sphere.intersect(nearest, ray))
    {
        nearest.material = material;
        nearest.type = LIGHT;

        return true;
    }

    return false;
}

