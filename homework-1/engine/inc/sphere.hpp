#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "math.hpp"

class Sphere
{
public:
    Sphere(float radius, vec3 origin);

    Sphere(float radius, float x, float y, float z);

    vec3 getOrigin() const;

    float getRadius() const;

    void setOrigin(const vec3 & origin);

private:
    float radius;
    vec3 origin;
};

#endif
