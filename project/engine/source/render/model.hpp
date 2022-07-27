#ifndef MODEL_HPP
#define MODEL_HPP

#include <d3d11_4.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <cassert>

#include "dx_res_ptr.hpp"
#include "globals.hpp"
#include "vertex_buffer.hpp"
#include "index_buffer.hpp"

namespace engine
{
class Model
{
public:
    Model(const std::string & model_filename);

    void bind();
    
    VertexBuffer<Vertex> vertex_buffer;
    IndexBuffer index_buffer;
    // uint32_t num_meshes;
};
} // namespace engine

#endif
