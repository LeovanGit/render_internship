#ifndef SHADER_HPP
#define SHADER_HPP

#include "win_def.hpp"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <iostream>

#include "dx_res_ptr.hpp"
#include "window.hpp"
#include "d3d.hpp"

#pragma comment( lib, "d3dcompiler.lib" )

#include "win_undef.hpp"

namespace engine
{
class Shader
{
public:
    Shader() = default;

    void init(const engine::windows::Window & win,
              WCHAR * vert_filename,
              WCHAR * frag_filename);

    DxResPtr<ID3D11VertexShader> vert_shader;
    DxResPtr<ID3D11PixelShader> frag_shader;

    DxResPtr<ID3D11InputLayout> m_input_layout;
};
} // namespace engine

#endif
