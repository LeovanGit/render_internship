#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include <iostream>
#include <cmath>

#include "glm.hpp"
#include "ray.hpp"
#include "intersection.hpp"

class Triangle
{
public:
    Triangle(glm::vec3 normal,
             glm::vec3 vertex_1,
             glm::vec3 vertex_2,
             glm::vec3 vertex_3);

    bool intersect(Intersection & nearest, Ray & ray) const;

    glm::vec3 normal;
    glm::vec3 vertex_1;
    glm::vec3 vertex_2;
    glm::vec3 vertex_3;
};

#endif
