#include "decal.hpp"

namespace engine
{
Decal::Decal(const glm::vec3 & position,
             const glm::vec2 & size,
             const glm::vec3 & albedo,
             const glm::mat4x4 & model) :
             position(position),
             size(size),
             albedo(albedo),
             model(model),
             model_inv(glm::inverse(model))
{}
}
