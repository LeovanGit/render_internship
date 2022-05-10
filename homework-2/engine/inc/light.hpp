#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "math.hpp"

class Light
{
public:
    Light(vec3 position, vec3 color);

vec3 position;
vec3 color;
};


#endif
