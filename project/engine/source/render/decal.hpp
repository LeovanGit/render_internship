#ifndef DECAL_HPP
#define DECAL_HPP

#include "glm.hpp"

namespace engine
{
class Decal
{
public:
    Decal(uint32_t transform_id,
          const glm::vec3 & posMS,
          const glm::vec2 & size,
          const glm::vec3 & albedo,
          const glm::vec3 & forward,
          const glm::vec3 & right,
          const glm::vec3 & up);

    uint32_t transform_id;
    
    glm::vec3 posMS;
    glm::vec2 size;
    glm::vec3 albedo;

    glm::vec3 forward;
    glm::vec3 right;
    glm::vec3 up;
};
} // namespace engine

#endif
