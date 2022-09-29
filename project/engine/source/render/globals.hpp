#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include "win_def.hpp"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <d3d11_4.h>
#include <cassert>
#include <iostream>
#include "glm.hpp"
#include "spdlog.h"

#include "dx_res_ptr.hpp"
#include "camera.hpp"
#include "light_system.hpp"
#include "transform_system.hpp"

#include "win_undef.hpp"

#pragma comment(lib, "dxgi.lib")

namespace engine
{
// size of cbuffers must be multiple of 16
struct PerFrameBufferData
{
    struct
    {
        glm::vec3 position;
        float padding_0;
        glm::vec3 radiance;
        float radius;
    } g_point_lights[4];

    struct
    {
        glm::vec3 direction;
        float padding_0;
        glm::vec3 radiance;
        float solid_angle;
    } g_dir_lights[1];

    int g_reflection_mips_count;
    int g_shadow_map_size;
    glm::vec<2, int> g_particles_atlas_size;
    
    glm::mat4 g_light_proj_view[24]; // 4 cubemaps
};

struct PerViewBufferData
{
    glm::mat4 g_proj_view;
    glm::mat4 g_view;
    glm::mat4 g_proj;
    glm::vec3 g_camera_pos;
    float g_EV_100;
    glm::vec4 g_frustum_corners[3];
};

struct PerMeshBufferData
{
    glm::mat4 g_mesh_to_model;

    // size of bool in HLSL is 4 bytes
    BOOL g_has_albedo_texture;
    BOOL g_has_roughness_texture;
    BOOL g_has_metalness_texture;
    BOOL g_has_normal_map;
    BOOL g_is_directx_style_normal_map;
    glm::vec3 padding_0;
    
    glm::vec3 g_albedo_default;
    float g_roughness_default;
    float g_metalness_default;    
    glm::vec3 padding_1;
};

struct PerEmissiveMeshBufferData
{
    glm::mat4 g_mesh_to_model;
};

struct PerShadowMeshBufferData
{
    glm::mat4 g_mesh_to_model;
};

struct PerShadowCubemapBufferData
{
    int cubemap_index;
    glm::vec3 padding_4;
};

// Singleton for global rendering resources
class Globals final
{
public:
    // deleted methods should be public for better error messages
    Globals(const Globals & other) = delete;
    void operator=(const Globals & other) = delete;

    static void init();

    static Globals * getInstance();

    static void del();

    void initD3D();

    void initSamplers();
    void bindSamplers();

    void initRasterizers();
    void bindRasterizer(bool is_double_sided = false);

    void initBlendStates();
    void bindBlendState(bool is_translucent = false);

    void initPerFrameBuffer();
    void setPerFrameBuffer(int g_reflection_mips_count,
                           int g_shadow_map_size,
                           const glm::vec<2, int> & g_particles_atlas_size);
    void updatePerFrameBuffer();

    void initPerViewBuffer();
    void setPerViewBuffer(const Camera & camera,
                          float EV_100);
    void updatePerViewBuffer();
    
    void initPerMeshBuffer();
    void setPerMeshBuffer(const glm::mat4 & g_mesh_to_model,
                          bool g_has_albedo_texture,
                          bool g_has_roughness_texture,
                          bool g_has_metalness_texture,
                          bool g_has_normal_map,
                          bool g_is_directx_style_normal_map,
                          const glm::vec3 & g_albedo_default,
                          float g_roughness_default,
                          float g_metalness_default);
    void updatePerMeshBuffer();

    void initPerEmissiveMeshBuffer();
    void setPerEmissiveMeshBuffer(const glm::mat4 & g_mesh_to_model);
    void updatePerEmissiveMeshBuffer();
    
    void initPerShadowMeshBuffer();
    void setPerShadowMeshBuffer(const glm::mat4 & g_mesh_to_model);
    void updatePerShadowMeshBuffer();

    void initPerShadowCubemapBuffer();
    void setPerShadowCubemapBuffer(int cubemap_index);
    void updatePerShadowCubemapBuffer();
    
    DxResPtr<IDXGIFactory5> factory5;
    DxResPtr<ID3D11Device5> device5;
    DxResPtr<ID3D11DeviceContext4> device_context4;
    DxResPtr<ID3D11Debug> device_debug;

    DxResPtr<ID3D11Buffer> per_frame_buffer;
    PerFrameBufferData per_frame_buffer_data;

    DxResPtr<ID3D11Buffer> per_view_buffer;
    PerViewBufferData per_view_buffer_data;
    
    DxResPtr<ID3D11Buffer> per_mesh_buffer;
    PerMeshBufferData per_mesh_buffer_data;

    DxResPtr<ID3D11Buffer> per_emissive_mesh_buffer;
    PerEmissiveMeshBufferData per_emissive_mesh_buffer_data;

    DxResPtr<ID3D11Buffer> per_shadow_mesh_buffer;
    PerShadowMeshBufferData per_shadow_mesh_buffer_data;

    DxResPtr<ID3D11Buffer> per_shadow_cubemap_buffer;
    PerShadowCubemapBufferData per_shadow_cubemap_buffer_data;
    
    DxResPtr<ID3D11SamplerState> wrap_sampler;
    DxResPtr<ID3D11SamplerState> clamp_sampler;
    DxResPtr<ID3D11SamplerState> comparison_sampler;

    DxResPtr<ID3D11RasterizerState> one_sided_rasterizer;
    DxResPtr<ID3D11RasterizerState> double_sided_rasterizer;

    DxResPtr<ID3D11BlendState> translucent_blend_state;

private:
    Globals() = default;
    ~Globals() = default;

    static Globals * instance;

    DxResPtr<IDXGIFactory> factory;
    DxResPtr<ID3D11Device> device;
    DxResPtr<ID3D11DeviceContext> device_context;
};
} // namespace engine

#endif
