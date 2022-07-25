#ifndef MODEL_HPP
#define MODEL_HPP

#include <d3d11_4.h>

#include "dx_res_ptr.hpp"
#include "globals.hpp"

namespace engine
{
struct Vertex
{
    float position[3];
    float uv[2];
};

class Model
{
public:
    Model(WCHAR * model_filename);

    void bind();
    
    DxResPtr<ID3D11Buffer> vertex_buffer;
};
} // namespace engine

#endif
