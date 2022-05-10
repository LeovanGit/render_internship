#ifndef INTERSECTION_HPP
#define INTERSECTION_HPP

#include <limits>

#include "math.hpp"
#include "material.hpp"

class Intersection
{
public:
    Intersection() = default;
    
    void reset();

    bool isExist();

float t;
vec3 point;
Material material;
};

#endif
