#include "intersection.hpp"

void Intersection::reset() { t = std::numeric_limits<float>::infinity(); }

bool Intersection::isExist()
{ 
    return t != std::numeric_limits<float>::infinity();
}
