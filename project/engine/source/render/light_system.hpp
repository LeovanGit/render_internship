#ifndef LIGHT_SYSTEM_HPP
#define LIGHT_SYSTEM_HPP

#include "spdlog.h"
#include <memory>
#include "glm.hpp"
#include <unordered_map>
#include <d3d11_4.h>
#include <dx_res_ptr.hpp>

#include "constants.hpp"
#include "globals.hpp"

namespace engine
{
class LightSystem
{
private:
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
    
public:    
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

    void initSquareViewport(int size);
    void bindSquareViewport();

    void initShadowMap(int size);
    void clearShadowMap();
    void bindShadowMap();

    void bindShadowMapSRV(int slot);
    
    const std::vector<DirectionalLight> & getDirectionalLights() const;
    const std::vector<PointLight> & getPointLights() const;
    
private:
    LightSystem() = default;
    ~LightSystem() = default;

    static LightSystem * instance;

    std::vector<DirectionalLight> directional_lights;
    std::vector<PointLight> point_lights;

    D3D11_VIEWPORT shadow_map_viewport;

    DxResPtr<ID3D11Texture2D> shadow_map;
    DxResPtr<ID3D11DepthStencilView> shadow_map_dsv;
    DxResPtr<ID3D11DepthStencilState> shadow_map_dss;
    DxResPtr<ID3D11ShaderResourceView> shadow_map_srv;
};
} // namespace engine

#endif
