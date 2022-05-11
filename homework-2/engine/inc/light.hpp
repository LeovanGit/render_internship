#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "math.hpp"
#include "sphere.hpp"

class PointLight
{
public:
    explicit PointLight(vec3 color,
                        float radius,
                        Sphere sphere,
                        Material material);

    bool intersect(Intersection & nearest, Ray & ray);

vec3 color;
float radius;

Sphere sphere;
Material material;
};

#endif
