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
private:
    struct GPUInstance
    {
        GPUInstance(const glm::mat4 & transform,
                    const glm::vec3 & radiance) :
                    transform(transform),
                    radiance(radiance)
        {}
        
        glm::mat4 transform;
        glm::vec3 radiance;
    };
public:
    struct Instance
    {
        Instance() = default;
        Instance(uint32_t transform_id,
                 const glm::vec3 & radiance) :
                 transform_id(transform_id),
                 radiance(radiance)
        {}

        uint32_t transform_id;
        glm::vec3 radiance;
    };
    
    struct Material
    {
        Material() = default;

        bool operator==(const Material & other)
        {
            return true;
        }

        bool operator!=(const Material & other)
        {
            return !(*this == other);
        }       
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
    VertexBuffer<GPUInstance> instance_buffer;
};
} // namespace engine

#endif
