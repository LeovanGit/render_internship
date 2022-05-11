#ifndef INTERSECTION_HPP
#define INTERSECTION_HPP

#include <limits>

#include "math.hpp"
#include "material.hpp"

#define SPHERE 0
#define LIGHT 1

class Intersection
{
public:
    Intersection() = default;
    
    void reset();

    bool isExist();

int type;
float t;
vec3 point;
vec3 normal;
Material material;
};

#endif
