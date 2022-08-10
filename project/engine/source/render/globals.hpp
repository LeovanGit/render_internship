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
};

struct PerMeshBufferData
{
    glm::mat4 g_mesh_to_model;
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

    void initD3D();

    void initSamplers();

    void setPerFrameBuffer(const Camera & camera);
    void updatePerFrameBuffer();

    void setPerMeshBuffer(const glm::mat4 & mesh_to_model);
    void updatePerMeshBuffer();

    DxResPtr<IDXGIFactory5> factory5;
    DxResPtr<ID3D11Device5> device5;
    DxResPtr<ID3D11DeviceContext4> device_context4;
    DxResPtr<ID3D11Debug> device_debug;

    DxResPtr<ID3D11Buffer> per_frame_buffer;
    PerFrameBufferData per_frame_buffer_data;

    DxResPtr<ID3D11Buffer> per_mesh_buffer;
    PerMeshBufferData per_mesh_buffer_data;

    DxResPtr<ID3D11SamplerState> sampler;

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
