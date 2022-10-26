#ifndef DECAL_HPP
#define DECAL_HPP

#include "glm.hpp"

namespace engine
{
class Decal
{
public:
    Decal(const glm::vec3 & position,
          const glm::vec2 & size,
          const glm::vec3 & albedo);

    glm::vec3 position;
    glm::vec2 size;
    glm::vec3 albedo;
};
} // namespace engine

#endif
