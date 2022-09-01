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
#include "triangle_octree.hpp"
#include "vertex.hpp"

namespace engine
{
class Model
{
public:
    struct MeshRange
    {
        uint32_t vertex_count;
        uint32_t index_count;
        uint32_t vertex_offset;
        uint32_t index_offset;

        glm::mat4 mesh_to_model;
    };
    
    Model(const std::string & model_filename);
    Model(std::vector<Vertex> & vertices,
          std::vector<int> & indices);

    void bind();

    std::vector<MeshRange> & getMeshRanges();
    MeshRange & getMeshRange(uint32_t index);
    std::vector<math::TriangleOctree> & getOctree();
    
protected:
    std::vector<MeshRange> meshes;
    VertexBuffer<Vertex> vertex_buffer;
    IndexBuffer index_buffer;
    std::vector<math::TriangleOctree> octrees;
};
} // namespace engine

#endif
