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

void LightSystem::addDirectionalLight(const glm::vec3 & direction,
                                      const glm::vec3 & radiance,
                                      float solid_angle)
{
    DirectionalLight directional_light(direction, radiance, solid_angle);
    
    directional_lights.push_back(directional_light);
}

void LightSystem::addPointLight(uint32_t transform_id,
                                const glm::vec3 & radiance,
                                float radius)
{
    PointLight point_light(transform_id, radiance, radius);

    point_lights.push_back(point_light);
}

glm::vec3 LightSystem::radianceFromIrradianceAtDistance(const glm::vec3 & irradiance,
                                                        float distance,
                                                        float radius)
{
    float sina = std::min(1.0f, radius / distance);
    float cosa = sqrtf(1.0f - sina * sina);
    float occupation = 1.0f - cosa;

    return irradiance / occupation;
}

const std::vector<LightSystem::DirectionalLight> & LightSystem::getDirectionalLights() const
{
    return directional_lights;
}

const std::vector<LightSystem::PointLight> & LightSystem::getPointLights() const
{
    return point_lights;
}

} // namespace engine
