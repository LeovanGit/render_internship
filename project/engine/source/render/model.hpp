#ifndef MODEL_HPP
#define MODEL_HPP

#include <d3d11_4.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <cassert>
#include <vector>

#include "dx_res_ptr.hpp"
#include "globals.hpp"
#include "vertex_buffer.hpp"
#include "index_buffer.hpp"

namespace engine
{
class Model
{
public:
    struct Mesh
    {
        uint32_t vertex_count;
        uint32_t index_count;
        uint32_t vertex_offset;
        uint32_t index_offset;
    };
    
    Model(const std::string & model_filename);

    void bind();

    std::vector<Mesh> & get_meshes();
    
protected:
    std::vector<Mesh> meshes;
    VertexBuffer<Vertex> vertex_buffer;
    IndexBuffer index_buffer;
};
} // namespace engine

#endif
