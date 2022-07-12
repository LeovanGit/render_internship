#ifndef SHADER_HPP
#define SHADER_HPP

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

#pragma comment(lib, "d3dcompiler.lib")

#include "win_undef.hpp"

namespace engine
{
class Shader
{
public:
    Shader(WCHAR * shader_filename,
           D3D11_INPUT_ELEMENT_DESC input_desc[]);

    void activate();

    DxResPtr<ID3D11VertexShader> vert_shader;
    DxResPtr<ID3D11PixelShader> frag_shader;
    DxResPtr<ID3D11InputLayout> input_layout;
};
} // namespace engine

#endif
