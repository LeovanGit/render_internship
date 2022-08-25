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
    glm::mat4 g_proj_view;
    glm::vec3 g_camera_pos;
    float g_EV_100;
    glm::vec4 g_frustum_corners[3];

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

    glm::vec3 g_radiance;
    float padding_0;
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

    void bind(const Camera & camera);

    void bindRasterizer(bool is_double_sided = false);

    void initD3D();

    void initSamplers();

    void initRasterizers();

    void setPerFrameBuffer(const Camera & camera);
    void updatePerFrameBuffer();

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

    void setPerEmissiveMeshBuffer(const glm::mat4 & g_mesh_to_model,
                                  const glm::vec3 & g_radiance);
    void updatePerEmissiveMeshBuffer();

    DxResPtr<IDXGIFactory5> factory5;
    DxResPtr<ID3D11Device5> device5;
    DxResPtr<ID3D11DeviceContext4> device_context4;
    DxResPtr<ID3D11Debug> device_debug;

    DxResPtr<ID3D11Buffer> per_frame_buffer;
    PerFrameBufferData per_frame_buffer_data;

    DxResPtr<ID3D11Buffer> per_mesh_buffer;
    PerMeshBufferData per_mesh_buffer_data;

    DxResPtr<ID3D11Buffer> per_emissive_mesh_buffer;
    PerEmissiveMeshBufferData per_emissive_mesh_buffer_data;
    
    DxResPtr<ID3D11SamplerState> sampler;

    DxResPtr<ID3D11RasterizerState> one_sided_rasterizer;
    DxResPtr<ID3D11RasterizerState> double_sided_rasterizer;

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
