#ifndef MESH_SYSTEM_HPP
#define MESH_SYSTEM_HPP

#include "spdlog.h"
#include <memory>

#include "opaque_instances.hpp"
#include "emissive_instances.hpp"
#include "dissolution_instances.hpp"
#include "model.hpp"
#include "matrices.hpp"
#include "transform_system.hpp"

namespace engine
{
class MeshSystem final
{
public:
    // deleted methods should be public for better error messages
    MeshSystem(const MeshSystem & other) = delete;
    void operator=(const MeshSystem & other) = delete;
    
    static void init();
    
    static MeshSystem * getInstance();

    static void del();

    void setShaders(std::shared_ptr<Shader> opaque,
                    std::shared_ptr<Shader> emissive,
                    std::shared_ptr<Shader> shadow,
                    std::shared_ptr<Shader> dissolve);

    void setTextures(std::shared_ptr<Texture> reflectance,
                     std::shared_ptr<Texture> irradiance,
                     std::shared_ptr<Texture> reflection,
                     std::shared_ptr<Texture> dissolve);
    
    void render();

    // for shadows from point lights
    void renderShadowCubeMaps(int cubemaps_count);

    bool findIntersection(const math::Ray & ray_ws,
                          math::MeshIntersection & nearest);
    
    template <class T>
    void addInstance(std::shared_ptr<Model> model,
                     const std::vector<struct T::Material> & materials,
                     const struct T::Instance & instance);

    template <>
    void addInstance<OpaqueInstances>(std::shared_ptr<Model> model,
                                      const std::vector<OpaqueInstances::Material> & materials,
                                      const OpaqueInstances::Instance & instance);

    template <>
    void addInstance<EmissiveInstances>(std::shared_ptr<Model> model,
                                        const std::vector<EmissiveInstances::Material> & materials,
                                        const EmissiveInstances::Instance & instance);

    template <>
    void addInstance<DissolutionInstances>(
        std::shared_ptr<Model> model,
        const std::vector<DissolutionInstances::Material> & materials,
        const DissolutionInstances::Instance & instance);

    OpaqueInstances opaque_instances;
    EmissiveInstances emissive_instances;
    DissolutionInstances dissolution_instances;

    std::shared_ptr<Shader> shadow_shader;

private:
    MeshSystem() = default;
    ~MeshSystem() = default;
    
    static MeshSystem * instance;
};

template <>
void MeshSystem::addInstance<OpaqueInstances>(std::shared_ptr<Model> model,
                                              const std::vector<OpaqueInstances::Material> & materials,
                                              const OpaqueInstances::Instance & instance)
{
    // try to find the same model
    for (auto & per_model : opaque_instances.per_model)
    {
        if (per_model.model == model)
        {
            // meshes and is materials are in the same order!
            for (uint32_t i = 0, size = materials.size(); i != size; ++i)
            {
                // try to find the same material
                for (auto & per_material: per_model.per_mesh[i].per_material)
                {
                    if (per_material.material == materials[i])
                    {
                        per_material.instances.push_back(instance);
                        goto next_material;
                    }
                }
                // if not found same materials in the mesh -> add new
                {
                OpaqueInstances::PerMaterial per_material;
                per_material.material = materials[i];
                per_material.instances.push_back(instance);
                per_model.per_mesh[i].per_material.push_back(per_material);
                } // block
                next_material:
                    continue;
            }
            goto end;
        }
    }
    // if not found same model -> add new
    {
    OpaqueInstances::PerModel per_model;
    per_model.model = model;
        
    for (auto & material : materials)
    {
        OpaqueInstances::PerMaterial per_material;
        per_material.material = material;
        per_material.instances.push_back(instance);    

        OpaqueInstances::PerMesh per_mesh;
        per_mesh.per_material.push_back(per_material);

        per_model.per_mesh.push_back(per_mesh);
    }
    
    opaque_instances.per_model.push_back(per_model);
    } // block
    
 end:
    opaque_instances.updateInstanceBuffers();
}

template <>
void MeshSystem::addInstance<EmissiveInstances>(std::shared_ptr<Model> model,
                                                const std::vector<EmissiveInstances::Material> & materials,
                                                const EmissiveInstances::Instance & instance)
{
    // try to find the same model
    for (auto & per_model : emissive_instances.per_model)
    {
        if (per_model.model == model)
        {
            // meshes and is materials are in the same order!
            for (uint32_t i = 0, size = materials.size(); i != size; ++i)
            {
                // try to find the same material
                for (auto & per_material: per_model.per_mesh[i].per_material)
                {
                    if (per_material.material == materials[i])
                    {
                        per_material.instances.push_back(instance);
                        goto next_material;
                    }
                }
                // if not found same materials in the mesh -> add new
                {
                EmissiveInstances::PerMaterial per_material;
                per_material.material = materials[i];
                per_material.instances.push_back(instance);
                per_model.per_mesh[i].per_material.push_back(per_material);
                } // block
                next_material:
                    continue;
            }
            goto end;
        }
    }
    // if not found same model -> add new
    {
    EmissiveInstances::PerModel per_model;
    per_model.model = model;
        
    for (auto & material : materials)
    {
        EmissiveInstances::PerMaterial per_material;
        per_material.material = material;
        per_material.instances.push_back(instance);    

        EmissiveInstances::PerMesh per_mesh;
        per_mesh.per_material.push_back(per_material);

        per_model.per_mesh.push_back(per_mesh);
    }
    
    emissive_instances.per_model.push_back(per_model);
    } // block
    
 end:
    emissive_instances.updateInstanceBuffers();
}

template <>
void MeshSystem::addInstance<DissolutionInstances>(
    std::shared_ptr<Model> model,
    const std::vector<DissolutionInstances::Material> & materials,
    const DissolutionInstances::Instance & instance)
{
    // try to find the same model
    for (auto & per_model : dissolution_instances.per_model)
    {
        if (per_model.model == model)
        {
            // meshes and is materials are in the same order!
            for (uint32_t i = 0, size = materials.size(); i != size; ++i)
            {
                // try to find the same material
                for (auto & per_material: per_model.per_mesh[i].per_material)
                {
                    if (per_material.material == materials[i])
                    {                                              
                        per_material.instances.push_back(instance);
                        goto next_material;
                    }
                }
                // if not found same materials in the mesh -> add new
                {
                DissolutionInstances::PerMaterial per_material;
                per_material.material = materials[i];
                per_material.instances.push_back(instance);
                per_model.per_mesh[i].per_material.push_back(per_material);
                } // block
                next_material:
                    continue;
            }
            goto end;
        }
    }
    // if not found same model -> add new
    {
    DissolutionInstances::PerModel per_model;
    per_model.model = model;
        
    for (auto & material : materials)
    {
        DissolutionInstances::PerMaterial per_material;
        per_material.material = material;
        per_material.instances.push_back(instance);

        DissolutionInstances::PerMesh per_mesh;
        per_mesh.per_material.push_back(per_material);

        per_model.per_mesh.push_back(per_mesh);
    }
    
    dissolution_instances.per_model.push_back(per_model);
    } // block
    
 end:
    dissolution_instances.updateInstanceBuffers();
}
} // namespace engine

#endif
