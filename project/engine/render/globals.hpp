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

#include "dx_res_ptr.hpp"

#include "win_undef.hpp"

#pragma comment(lib, "dxgi.lib")

namespace engine
{
struct Vertex
{
    float x;
    float y;
    float z;
    float color[4];
};

// Singleton for global rendering resources
// NAIVE SINGLETON -> not for multithreading!!!
class Globals final
{
public:
    // deleted methods should be public for better error messages
    Globals(const Globals & other) = delete;
    void operator=(const Globals & other) = delete;

    static void initGlobals();

    static Globals * getInstance();

    static void deleteGlobals();

    void initD3D();

    void initVBO();

    DxResPtr<IDXGIFactory5> factory5;
    DxResPtr<ID3D11Device5> device5;
    DxResPtr<ID3D11DeviceContext4> device_context4;
    DxResPtr<ID3D11Debug> device_debug;
    DxResPtr<ID3D11Buffer> vbo;

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
