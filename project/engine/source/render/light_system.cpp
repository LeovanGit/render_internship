#include "light_system.hpp"

namespace engine
{
LightSystem * LightSystem::instance = nullptr;

void LightSystem::init()
{
    if (!instance) instance = new LightSystem();
    else spdlog::error("LightSystem::init() was called twice!");
}

LightSystem * LightSystem::getInstance()
{
    return instance;
}

void LightSystem::del()
{
    if (instance)
    {
        delete instance;
        instance = nullptr;
    }
    else spdlog::error("LightSystem::del() was called twice!");
}

void LightSystem::addDirectionalLight(const glm::vec3 & radiance,
                                      const glm::vec3 & direction,
                                      float solid_angle)
{
    DirectionalLight directional_light(radiance, direction, solid_angle);
    
    directional_lights.push_back(directional_light);
}

void LightSystem::addPointLight(const glm::vec3 & position,
                                const glm::vec3 & radiance,
                                float radius)
{
    PointLight point_light(position, radiance, radius);

    point_lights.push_back(point_light);
}
} // namespace engine
