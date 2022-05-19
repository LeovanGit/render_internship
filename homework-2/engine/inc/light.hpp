#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "glm.hpp"
#include "sphere.hpp"

class PointLight
{
public:
    explicit PointLight(glm::vec3 color,
                        float radius,
                        Sphere sphere,
                        Material material);

    bool intersect(Intersection & nearest, Ray & ray);

glm::vec3 color;
float radius;

Sphere sphere;
Material material;
};

class DirectionalLight
{
public:
    explicit DirectionalLight(glm::vec3 color,
                              glm::vec3 direction,
                              Sphere sphere,
                              Material material);

    bool intersect(Intersection & nearest, Ray & ray);

glm::vec3 color;
glm::vec3 direction;

Sphere sphere;
Material material;
};

class SpotLight
{
public:
    explicit SpotLight(glm::vec3 color,
                       float radius,
                       float angle,
                       glm::vec3 direction,
                       Sphere sphere,
                       Material material);

    bool intersect(Intersection & nearest, Ray & ray);

glm::vec3 color;
float radius;
float angle;
glm::vec3 direction;

Sphere sphere;
Material material;
};

#endif
