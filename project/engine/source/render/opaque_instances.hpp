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
        Instance(const glm::mat4 & transform) : transform(transform) {}
        glm::mat4 transform;
    };
    
    struct Material
    {
        Material() = default;
        Material(std::shared_ptr<Texture> albedo,
                 std::shared_ptr<Texture> roughness = nullptr,
                 std::shared_ptr<Texture> metalness = nullptr,
                 std::shared_ptr<Texture> normal = nullptr,
                 float roughness_default = -1.0f,
                 float metalness_default = -1.0f) :
                 albedo(albedo),
                 roughness(roughness),
                 metalness(metalness),
                 normal(normal),
                 roughness_default(roughness_default),
                 metalness_default(metalness_default)
        {}

        bool operator==(const Material & other)
        {
            return (albedo == other.albedo &&
                    roughness == other.roughness &&
                    metalness == other.metalness &&
                    normal == other.normal);
        }

        bool operator!=(const Material & other)
        {
            return !(*this == other);
        }
        
        std::shared_ptr<Texture> albedo;
        std::shared_ptr<Texture> roughness;
        std::shared_ptr<Texture> metalness;
        std::shared_ptr<Texture> normal;

        float roughness_default;
        float metalness_default;
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
    VertexBuffer<Instance> instance_buffer;
};
} // namespace engine

#endif
