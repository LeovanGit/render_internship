#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include "win_def.hpp"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <d3d11_4.h>
#include <cassert>
#include <iostream>
#include <array>
#include "glm.hpp"

#include "dx_res_ptr.hpp"
#include "camera.hpp"

#include "win_undef.hpp"

#pragma comment(lib, "dxgi.lib")

namespace engine
{
struct Vertex
{
    float position[3];
    float uv[2];
};

struct ConstBufferData
{
    glm::mat4 proj_view;
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

    void initVBO();

    void setConstBuffer(const Camera & camera);

    DxResPtr<IDXGIFactory5> factory5;
    DxResPtr<ID3D11Device5> device5;
    DxResPtr<ID3D11DeviceContext4> device_context4;
    DxResPtr<ID3D11Debug> device_debug;

    DxResPtr<ID3D11Buffer> vbo;

    DxResPtr<ID3D11Buffer> const_buffer;
    ConstBufferData const_buffer_data;

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
