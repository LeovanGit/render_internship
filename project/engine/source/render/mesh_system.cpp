#include "mesh_system.hpp"

namespace engine
{
MeshSystem * MeshSystem::instance = nullptr;

void MeshSystem::init()
{
    if (!instance) instance = new MeshSystem();
    else spdlog::error("MeshSystem::init() was called twice!");
}

MeshSystem * MeshSystem::getInstance()
{
    return instance;
}

void MeshSystem::del()
{
    if (instance)
    {
        delete instance;
        instance = nullptr;
    }
    else spdlog::error("MeshSystem::del() was called twice!");
}

void MeshSystem::render() { opaque_instances.render(); }

void MeshSystem::addInstance(std::shared_ptr<Model> model,
                             std::vector<OpaqueInstances::Material> & materials,
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
                        if (per_material.material.texture == material.texture)
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
} // namespace engine
