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
    struct Vertex
    {
        glm::vec3 position;
        glm::vec2 uv;
    };

    struct MeshRange
    {
        uint32_t vertex_count;
        uint32_t index_count;
        uint32_t vertex_offset;
        uint32_t index_offset;

        glm::mat4 mesh_to_model;
    };
    
    Model(const std::string & model_filename);
    Model(Vertex * vertices,
          uint32_t vertices_size,
          int * indices,
          uint32_t indices_size);

    void bind();

    MeshRange & get_mesh(uint32_t index);
    
protected:
    std::vector<MeshRange> meshes;
    VertexBuffer<Vertex> vertex_buffer;
    IndexBuffer index_buffer;
};
} // namespace engine

#endif
