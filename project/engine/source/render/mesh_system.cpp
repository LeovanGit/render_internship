#include "mesh_system.hpp"

namespace engine
{
void MeshSystem::render() { opaque_instances.render(); }

void MeshSystem::addModel(Model * model,
                          const OpaqueInstances::Material & material,
                          const math::Transform & transform)
{
    OpaqueInstances::Instance instance;
    instance.transform = transform.toMat4();

    // try to find the same model
    for (auto & per_model : opaque_instances.per_model)
    {
        if (per_model.model == model)
        {
            for (auto & per_mesh : per_model.per_mesh)
            {
                // try to find the same texture
                for (auto & per_material : per_mesh.per_material)
                {
                    if (per_material.material.texture == material.texture)
                    {
                        per_material.instances.push_back(instance);
                    
                        goto end;
                    }
                }
            }

            // if not found same texture -> add new
            OpaqueInstances::PerMaterial per_material;
            per_material.material = material;
            per_material.instances.push_back(instance);

            OpaqueInstances::PerMesh per_mesh;
            per_mesh.per_material.push_back(per_material);
        
            per_model.per_mesh.push_back(per_mesh);

            // also add meshes_range
            auto & src_meshes = model->get_meshes();
            auto & dst_meshes = per_model.model->get_meshes();

            dst_meshes.insert(dst_meshes.end(),
                              src_meshes.begin(),
                              src_meshes.end());

            goto end;
        }
    }

    // if not found same model -> add new
    {
        OpaqueInstances::PerMaterial per_material;
        per_material.material = material;
        per_material.instances.push_back(instance);

        OpaqueInstances::PerMesh per_mesh;
        per_mesh.per_material.push_back(per_material);

        OpaqueInstances::PerModel per_model;
        per_model.model = model;
        per_model.per_mesh.push_back(per_mesh);

        opaque_instances.per_model.push_back(per_model);
    }
    
 end:
    opaque_instances.updateInstanceBuffers();
}
} // namespace engine
