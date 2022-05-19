#ifndef INTERSECTION_HPP
#define INTERSECTION_HPP

#include <limits>

#include "glm.hpp"
#include "material.hpp"

#define SPHERE 0
#define LIGHT 1
#define PLANE 2
#define TRIANGLE 3

class Intersection
{
public:
    Intersection() = default;
    
    void reset();

    bool isExist();

int type;

float t;
glm::vec3 point;
glm::vec3 normal;

Material material;
};

#endif
