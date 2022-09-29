#include "particle.hpp"

namespace engine
{
Particle::Particle(const glm::vec3 & position,
                   const glm::vec2 & size,
                   float thickness,
                   float angle,
                   const glm::vec4 tint,
                   float lifetime) :
                   position(position),
                   size(size),
                   thickness(thickness),
                   angle(angle),
                   tint(tint),
                   lifetime(lifetime)
{}
} // namespace enigne
