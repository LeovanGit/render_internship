#include "particle.hpp"

namespace engine
{
Particle::Particle(const glm::vec3 & position,
                   const glm::vec2 & size,
                   float thickness,
                   float angle,
                   const glm::vec4 tint,
                   bool is_disappears) :
                   position(position),
                   size(size),
                   thickness(thickness),
                   angle(angle),
                   tint(tint),
                   is_disappears(is_disappears)
{}
} // namespace enigne
