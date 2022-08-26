#ifndef LIGHT_SYSTEM_HPP
#define LIGHT_SYSTEM_HPP

#include "spdlog.h"
#include <memory>
#include "glm.hpp"
#include <unordered_map>

#include "constants.hpp"

namespace engine
{
class LightSystem
{
public:
    class DirectionalLight
    {
    public:
        DirectionalLight(const glm::vec3 & direction,
                         const glm::vec3 & radiance,                         
                         float solid_angle) :
                         direction(direction),
                         radiance(radiance),
                         solid_angle(solid_angle)
        {}
        
        glm::vec3 direction;
        glm::vec3 radiance;
        float solid_angle;
    };
    
    class PointLight
    {
    public:
        PointLight(uint32_t transform_id,
                   const glm::vec3 & radiance,
                   float radius) :
                   transform_id(transform_id),
                   radiance(radiance),
                   radius(radius)
        {}

        uint32_t transform_id;
        glm::vec3 radiance;
        float radius;
    };
    
    // deleted methods should be public for better error messages
    LightSystem(const LightSystem & other) = delete;
    void operator=(const LightSystem & other) = delete;

    static void init();

    static LightSystem * getInstance();

    static void del();

    void addDirectionalLight(const glm::vec3 & direction,
                             const glm::vec3 & radiance,
                             float solid_angle);

    void addPointLight(uint32_t transform_id,
                       const glm::vec3 & radiance,
                       float radius);

    // easy way to set the radiance of the light source:
    // set it irradiance at some distance.
    // This func will calculate light source radiance from this values
    static glm::vec3 radianceFromIrradianceAtDistance(const glm::vec3 & irradiance,
                                                      float distance,
                                                      float radius);
    
    std::vector<DirectionalLight> directional_lights;
    std::vector<PointLight> point_lights;
    
private:
    LightSystem() = default;
    ~LightSystem() = default;

    static LightSystem * instance;
};
} // namespace engine

#endif
