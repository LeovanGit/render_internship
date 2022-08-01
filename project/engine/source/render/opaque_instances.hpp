#ifndef OPAQUE_INSTANCES_HPP
#define OPAQUE_INSTANCES_HPP

#include <vector>
#include <memory>

#include "globals.hpp"
#include "shader_manager.hpp"
#include "texture_manager.hpp"
#include "vertex_buffer.hpp"
#include "model.hpp"

namespace engine
{
class OpaqueInstances
{
public:
    struct Instance
    {
        Instance() = default;
        Instance(glm::mat4 transform) : transform(transform) {}        
        glm::mat4 transform;
    };
    
    struct Material
    {
        Material() = default;
        Material(Texture * texture) : texture(texture) {}        
        Texture * texture;
    };

    struct PerMaterial
    {
        Material material;
        std::vector<Instance> instances;
    };

    struct PerMesh
    {
        std::vector<PerMaterial> per_material;
    };

    struct PerModel
    {
        Model * model;
        std::vector<PerMesh> per_mesh;
    };

    void updateInstanceBuffers();
    
    void render();

    std::vector<PerModel> per_model;
    VertexBuffer<Instance> instance_buffer;
};
} // namespace engine

#endif
