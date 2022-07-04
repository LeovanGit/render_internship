#ifndef D3D_HPP
#define D3D_HPP

#include "win_def.hpp"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <d3d11_4.h>
#include <d3dcompiler.h>

namespace engine
{
struct Vertex
{
    float x;
    float y;
    float z;
    float color[4];
};

extern ID3D11Device5 * s_device;
extern ID3D11DeviceContext4 * s_device_context;
extern IDXGIFactory5 * s_factory;

extern ID3D11Buffer * s_vbo; // Vertex Buffer Object
extern ID3D11InputLayout * s_input_layout;
} // namespace engine

#include "win_undef.hpp"

#endif
