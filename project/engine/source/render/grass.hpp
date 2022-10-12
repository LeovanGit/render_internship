#ifndef GRASS_HPP
#define GRASS_HPP

#include "glm.hpp"

namespace engine
{
class Grass
{
public:
    Grass(const glm::vec3 & position,
          const glm::vec2 & size);
    
    glm::vec3 position;
    glm::vec2 size;
};
} // namespace engine

#endif
