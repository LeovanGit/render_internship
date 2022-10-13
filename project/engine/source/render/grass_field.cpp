#include "grass_field.hpp"

namespace
{
constexpr float MIN_GRASS_SIZE = 4.0f;
constexpr float MAX_GRASS_SIZE = 10.0f;
} // namespace 

namespace engine
{
GrassField::GrassField(const glm::vec3 & position,
                       const glm::vec2 & size) :
                       position(position),
                       size(size)
{
    initGrass();
}

const std::vector<Grass> & GrassField::getGrass() const
{
    return grass;
}

void GrassField::initGrass()
{
    std::vector<glm::vec2> positions = math::poissonDiscSampling(size.x,
                                                                 size.y,
                                                                 4.0f);
    
    for (auto & pos : positions)
    {
        glm::vec2 grass_size(math::randomFromRange(MIN_GRASS_SIZE,
                                                   MAX_GRASS_SIZE));

        glm::vec3 grass_pos = glm::vec3(pos.x, grass_size.y / 2.0f, pos.y) +
                              position -
                              glm::vec3(size.x, 0.0f, size.y) / 2.0f;
        
        
        grass.push_back(Grass(grass_pos, grass_size));
    }
}
} // namespace engine
