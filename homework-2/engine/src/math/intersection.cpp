#include "intersection.hpp"

void math::Intersection::reset() 
{ 
    t = std::numeric_limits<float>::infinity();
}

bool math::Intersection::isExist() const
{ 
    return t != std::numeric_limits<float>::infinity();
}
