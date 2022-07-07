#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include "win_def.hpp"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <d3d11_4.h>
#include <cassert>
#include <iostream>

#include "dx_res_ptr.hpp"
#include "d3d.hpp"

#include "win_undef.hpp"

#pragma comment(lib, "dxgi.lib")

namespace engine
{
// Singleton for global rendering resources
class Globals
{
public:
    Globals() = default;

    void initD3D();

    void initVBO();

    DxResPtr<IDXGIFactory> m_factory;
    DxResPtr<IDXGIFactory5> m_factory5;

    DxResPtr<ID3D11Device> m_device;
    DxResPtr<ID3D11Device5> m_device5;

    DxResPtr<ID3D11DeviceContext> m_device_context;
    DxResPtr<ID3D11DeviceContext4> m_device_context4;

    DxResPtr<ID3D11Debug> m_device_debug;

    DxResPtr<ID3D11Buffer> m_vbo;
};
} // namespace engine

#endif
