#ifndef SHADER_HPP
#define SHADER_HPP

#include <iostream>

#include "dx_res_ptr.hpp"
#include "window.hpp"
#include "d3d.hpp"

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
};
} // namespace engine

#endif
