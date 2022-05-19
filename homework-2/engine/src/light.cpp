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

DirectionalLight::DirectionalLight(glm::vec3 color,
                                   glm::vec3 direction,
                                   Sphere sphere,
                                   Material material) :
                                   color(color),
                                   direction(direction),
                                   sphere(sphere),
                                   material(material)
{}

bool DirectionalLight::intersect(Intersection & nearest, Ray & ray)
{
    if (sphere.intersect(nearest, ray))
    {
        nearest.material = material;
        nearest.type = LIGHT;

        return true;
    }

    return false;
}

SpotLight::SpotLight(glm::vec3 color,
                     float radius,
                     float angle,
                     glm::vec3 direction,
                     Sphere sphere,
                     Material material) :
                     color(color),
                     radius(radius),
                     angle(angle),
                     direction(direction),
                     sphere(sphere),
                     material(material)
{}

bool SpotLight::intersect(Intersection & nearest, Ray & ray)
{
    if (sphere.intersect(nearest, ray))
    {
        nearest.material = material;
        nearest.type = LIGHT;

        return true;
    }

    return false;
}

