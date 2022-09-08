#ifndef SHADER_HPP
#define SHADER_HPP

// for graphics debugging tools (like RenderDoc)
#if (!NDEBUG || defined(FORCE_SKIP_SHADER_OPTIMIZATIONS))
    #define SKIP_SHADER_OPTIMIZATIONS D3DCOMPILE_DEBUG |\
                                      D3DCOMPILE_SKIP_OPTIMIZATION
#else
    #define SKIP_SHADER_OPTIMIZATIONS 0
#endif

#include "win_def.hpp"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <iostream>
#include "glm.hpp"

#include "dx_res_ptr.hpp"
#include "d3d11_4.h"
#include <d3dcompiler.h>
#include "globals.hpp"
#include "shader_includer.hpp"

#pragma comment(lib, "d3dcompiler.lib")

#include "win_undef.hpp"

namespace engine
{
class Shader
{
public:
    Shader(const std::string & shader_path,
           D3D11_INPUT_ELEMENT_DESC input_desc[] = nullptr,
           uint32_t input_desc_size = 0,
           bool vertex_shader = true,
           bool geometry_shader = false,
           bool fragment_shader = true);

    void bind();

    DxResPtr<ID3D11VertexShader> vert_shader;
    DxResPtr<ID3D11PixelShader> frag_shader;
    DxResPtr<ID3D11GeometryShader> geom_shader;
    DxResPtr<ID3D11InputLayout> input_layout;
};
} // namespace engine

#endif
