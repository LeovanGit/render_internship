#include "grass_field.hpp"

namespace
{
constexpr float MIN_GRASS_SIZE = 10.0f;
constexpr float MAX_GRASS_SIZE = 10.0f;
} // namespace 

namespace engine
{
GrassField::GrassField(const glm::vec3 & position,
                       const glm::vec2 & size,
                       uint32_t grass_count) :
                       position(position),
                       size(size)
{
    initGrass(grass_count);
}

const std::vector<Grass> & GrassField::getGrass() const
{
    return grass;
}

void GrassField::initGrass(uint32_t grass_count)
{
    for (uint32_t i = 0; i != grass_count; ++i)
    {
        glm::vec2 grass_size(math::randomFromRange(MIN_GRASS_SIZE,
                                                   MAX_GRASS_SIZE));
        
        glm::vec3 grass_pos(math::randomFromRange(position.x - size.x / 2.0f,
                                                  position.x + size.x / 2.0f),
                            position.y + grass_size.y / 2.0f,
                            math::randomFromRange(position.z - size.y / 2.0f,
                                                  position.z + size.y / 2.0f));
        
        grass.push_back(Grass(grass_pos, grass_size));
    }
}
} // namespace engine
