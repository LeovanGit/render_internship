#ifndef MESH_SYSTEM_HPP
#define MESH_SYSTEM_HPP

#include "spdlog.h"
#include <memory>

#include "opaque_instances.hpp"
#include "emissive_instances.hpp"
#include "model.hpp"
#include "matrices.hpp"
#include "transform_system.hpp"

namespace engine
{
class MeshSystem
{
public:
    // deleted methods should be public for better error messages
    MeshSystem(const MeshSystem & other) = delete;
    void operator=(const MeshSystem & other) = delete;
    
    static void init();
    
    static MeshSystem * getInstance();

    static void del();

    void render();

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

    OpaqueInstances opaque_instances;
    EmissiveInstances emissive_instances;

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
            for (auto & material : materials)
            {
                for (auto & per_mesh : per_model.per_mesh)
                {
                    // try to find the same texture
                    for (auto & per_material : per_mesh.per_material)
                    {
                        if (per_material.material == material)
                        {
                            per_material.instances.push_back(instance);

                            goto next;
                        }
                    }
                }
                {
                // if not found same texture -> add new
                OpaqueInstances::PerMaterial per_material;
                per_material.material = material;
                per_material.instances.push_back(instance);

                OpaqueInstances::PerMesh per_mesh;
                per_mesh.per_material.push_back(per_material);
        
                per_model.per_mesh.push_back(per_mesh);

                // also add meshes_range
                auto & src_meshes = model->getMeshRanges();
                auto & dst_meshes = per_model.model->getMeshRanges();

                dst_meshes.insert(dst_meshes.end(),
                                  src_meshes.begin(),
                                  src_meshes.end());
                } // block
                
            next:
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
            for (auto & material : materials)
            {
                for (auto & per_mesh : per_model.per_mesh)
                {
                    // try to find the same texture
                    for (auto & per_material : per_mesh.per_material)
                    {
                        if (per_material.material == material)
                        {
                            per_material.instances.push_back(instance);

                            goto next;
                        }
                    }
                }
                {
                    // if not found same texture -> add new
                    EmissiveInstances::PerMaterial per_material;
                    per_material.material = material;
                    per_material.instances.push_back(instance);

                    EmissiveInstances::PerMesh per_mesh;
                    per_mesh.per_material.push_back(per_material);
        
                    per_model.per_mesh.push_back(per_mesh);

                    // also add meshes_range
                    auto & src_meshes = model->getMeshRanges();
                    auto & dst_meshes = per_model.model->getMeshRanges();

                    dst_meshes.insert(dst_meshes.end(),
                                      src_meshes.begin(),
                                      src_meshes.end());
                } // block
                
            next:
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
} // namespace engine

#endif
