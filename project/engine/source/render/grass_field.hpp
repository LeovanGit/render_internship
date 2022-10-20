#ifndef GRASS_FIELD_HPP
#define GRASS_FIELD_HPP

#include "glm.hpp"
#include <vector>

#include "random.hpp"
#include "grass.hpp"

namespace engine
{
class GrassField
{
public:
    GrassField(const glm::vec3 & position,
               const glm::vec2 & size);
    
    glm::vec3 position;
    glm::vec2 size;

    const std::vector<Grass> & getGrass() const;
    
private:
    void initGrass();

    std::vector<Grass> grass;
};
} // namespace engine

#endif
