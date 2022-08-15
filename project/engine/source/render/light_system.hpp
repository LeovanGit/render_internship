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
        DirectionalLight(const glm::vec3 & radiance,
                         const glm::vec3 & direction,
                         float solid_angle) :
                         radiance(radiance),
                         direction(direction),
                         solid_angle(solid_angle)
        {}
        
        glm::vec3 radiance;
        glm::vec3 direction;
        float solid_angle;
    };
    
    class PointLight
    {
    public:
        PointLight(const glm::vec3 & position,
                   const glm::vec3 & radiance,
                   float radius) :
                   position(position),
                   radiance(radiance),
                   radius(radius)
        {}

        glm::vec3 position;
        glm::vec3 radiance;
        float radius;
    };
    
    // deleted methods should be public for better error messages
    LightSystem(const LightSystem & other) = delete;
    void operator=(const LightSystem & other) = delete;

    static void init();

    static LightSystem * getInstance();

    static void del();

    void addDirectionalLight(const glm::vec3 & radiance,
                             const glm::vec3 & direction,
                             float solid_angle);

    void addPointLight(const glm::vec3 & position,
                       const glm::vec3 & radiance,
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
