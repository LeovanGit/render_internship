#ifndef MATH_HPP
#define MATH_HPP

#include <cmath>

class vec3
{
public:
    explicit vec3(float x = 0, float y = 0, float z = 0) :
        x(x), y(y), z(z)
    {}

    inline float getX() const { return x; }

    inline float getY() const { return y; }

    inline float getZ() const { return z; }

    inline float getLength() const
    {
        return sqrt(x * x + y * y + z * z);
    }

    inline vec3 getNormalized() const
    {
        float len = this->getLength();
        return vec3(x / len, y / len, z / len);
    };    

    inline vec3 operator*(const float k) const
    {
        return vec3(k * x, k * y, k * z);
    }

    inline vec3 operator+(const vec3 & other) const
    {
        return vec3(x + other.x, y + other.y, z + other.z);
    }

    inline vec3 operator-(const vec3 & other) const
    {
        return vec3(x - other.x, y - other.y, z - other.z);
    }

private:
    float x;
    float y;
    float z;
};

float dot(const vec3 & v1, const vec3 & v2);

#endif

