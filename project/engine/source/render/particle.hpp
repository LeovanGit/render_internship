#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include "glm.hpp"

namespace engine
{
class Particle
{
public:
    Particle(const glm::vec3 & position,
             const glm::vec2 & size,
             float thickness,
             float angle,
             const glm::vec4 tint,
             float lifetime = 0.0f);
    
    glm::vec3 position;
    glm::vec2 size;
    float thickness;
    float angle;
    glm::vec4 tint;
    float lifetime; // [0; 1]
};
} // namespace engine

#endif
