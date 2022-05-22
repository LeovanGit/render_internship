#ifndef INTERSECTION_HPP
#define INTERSECTION_HPP

#include <limits>

#include "glm.hpp"

namespace math
{
class Intersection
{
public:
    Intersection() = default;
    
    void reset();

    bool isExist() const;

float t;
glm::vec3 point;
glm::vec3 normal;
};
} // namespace math

#endif
