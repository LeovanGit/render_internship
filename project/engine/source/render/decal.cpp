#include "decal.hpp"

namespace engine
{
Decal::Decal(uint32_t transform_id,
             const glm::vec3 & posMS,
             const glm::vec2 & size,
             const glm::vec3 & albedo,
             const glm::vec3 & forward,
             const glm::vec3 & right,
             const glm::vec3 & up) :
             transform_id(transform_id),
             posMS(posMS),
             size(size),
             albedo(albedo),
             forward(forward),
             right(right),
             up(up)
{}
}
