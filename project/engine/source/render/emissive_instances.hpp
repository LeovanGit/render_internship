#ifndef EMISSIVE_INSTANCES_HPP
#define EMISSIVE_INSTANCES_HPP

#include <vector>
#include <memory>

#include "globals.hpp"
#include "shader_manager.hpp"
#include "texture_manager.hpp"
#include "vertex_buffer.hpp"
#include "model.hpp"
#include "transform_system.hpp"

namespace engine
{
class EmissiveInstances
{
public:
    struct Instance
    {
        Instance() = default;
        Instance(uint32_t transform_id) : transform_id(transform_id) {}

        uint32_t transform_id;
    };
    
    struct Material
    {
        Material() = default;
        Material(const glm::vec3 & radiance) :
                 radiance(radiance)
        {}

        bool operator==(const Material & other)
        {
            return (radiance == other.radiance);
        }

        bool operator!=(const Material & other)
        {
            return !(*this == other);
        }
        
        glm::vec3 radiance;
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
        std::shared_ptr<Model> model;
        std::vector<PerMesh> per_mesh;
    };

    void updateInstanceBuffers();
    
    void render();

    std::vector<PerModel> per_model;
    VertexBuffer<glm::mat4> instance_buffer;
};
} // namespace engine

#endif