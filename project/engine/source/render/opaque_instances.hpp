#ifndef OPAQUE_INSTANCES_HPP
#define OPAQUE_INSTANCES_HPP

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
class OpaqueInstances
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
        Material(std::shared_ptr<Texture> albedo,
                 std::shared_ptr<Texture> roughness = nullptr,
                 std::shared_ptr<Texture> metalness = nullptr,
                 std::shared_ptr<Texture> normal = nullptr,
                 bool is_directx_style_normal_map = true,
                 bool is_double_sided = false,
                 glm::vec3 albedo_default = glm::vec3(-1.0f),
                 float roughness_default = -1.0f,
                 float metalness_default = -1.0f) :
                 albedo(albedo),
                 roughness(roughness),
                 metalness(metalness),
                 normal(normal),
                 is_directx_style_normal_map(is_directx_style_normal_map),
                 albedo_default(albedo_default),
                 roughness_default(roughness_default),
                 metalness_default(metalness_default),
                 is_double_sided(is_double_sided)
        {}

        bool operator==(const Material & other)
        {
            if (static_cast<bool>(albedo))
            {
                if (albedo != other.albedo) return false;
            }                
            else
            {
                if (albedo_default != other.albedo_default) return false;
            }
            
            if (static_cast<bool>(roughness))
            {
                if (roughness != other.roughness) return false;
            }                
            else
            {
                if (roughness_default != other.roughness_default) return false;
            }
            
            if (static_cast<bool>(metalness))
            {
                if (metalness != other.metalness) return false;
            }                
            else
            {
                if (metalness_default != other.metalness_default) return false;
            }
            
            if (static_cast<bool>(normal))
            {
                if (normal != other.normal) return false;
            }

            if (is_double_sided != other.is_double_sided) return false;

            if (is_directx_style_normal_map != other.is_directx_style_normal_map) return false;

            return true;
        }

        bool operator!=(const Material & other)
        {
            return !(*this == other);
        }
        
        std::shared_ptr<Texture> albedo;
        std::shared_ptr<Texture> roughness;
        std::shared_ptr<Texture> metalness;
        std::shared_ptr<Texture> normal;

        glm::vec3 albedo_default;
        float roughness_default;
        float metalness_default;

        bool is_double_sided;
        bool is_directx_style_normal_map;
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
