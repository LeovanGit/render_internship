#include "decal.hpp"

namespace engine
{
Decal::Decal(const glm::vec3 & position,
             const glm::vec2 & size,
             const glm::vec3 & albedo) :
             position(position),
             size(size),
             albedo(albedo)
{}
}
