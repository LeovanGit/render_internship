#ifndef PLANE_HPP
#define PLANE_HPP

#include <iostream>
#include <cmath>

#include "glm.hpp"
#include "ray.hpp"
#include "intersection.hpp"

class Plane
{
public:
    Plane(glm::vec3 a, glm::vec3 b, glm::vec3 c);

    bool intersect(Intersection & nearest, Ray & ray);

    glm::vec3 a;
    glm::vec3 b;
    glm::vec3 c;
    glm::vec3 normal;
};

#endif
