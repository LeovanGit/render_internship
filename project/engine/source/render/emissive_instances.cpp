#include "emissive_instances.hpp"

namespace engine
{
void EmissiveInstances::updateInstanceBuffers()
{
    TransformSystem * trans_system = TransformSystem::getInstance();
    
    uint32_t total_instances = 0;
    
    for (auto & per_model : per_model)
        for (auto & per_mesh : per_model.per_mesh)
            for (auto & per_material : per_mesh.per_material)
                total_instances += uint32_t(per_material.instances.size());

    if (total_instances == 0) return;

    instance_buffer.init(total_instances);
    D3D11_MAPPED_SUBRESOURCE mapped = instance_buffer.map();
    GPUInstance * dst = static_cast<GPUInstance *>(mapped.pData);

    uint32_t copied_count = 0;
    for (auto & per_model : per_model)
    {
        for (auto & per_mesh : per_model.per_mesh)
        {
            for (auto & per_material : per_mesh.per_material)
            {
                uint32_t instances_size = per_material.instances.size();
                for (uint32_t i = 0; i != instances_size; ++i)
                {
                    dst[copied_count++] = GPUInstance(
                        trans_system->
                            transforms[per_material.instances[i].transform_id].toMat4(),
                        per_material.instances[i].radiance);
                }
            }
        }
    }

    instance_buffer.unmap();
}

void EmissiveInstances::render()
{
    if (instance_buffer.get_size() == 0) return;

    Globals * globals = Globals::getInstance();

    globals->bindDefaultBlendState();
    
    globals->device_context4->
        IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    shader->bind();
    instance_buffer.bind(1);

    uint32_t rendered_instances = 0;
    
    for (auto & per_model: per_model)
    {
        if (per_model.model == nullptr) continue;

        // bind vertex and index buffers
        per_model.model->bind();

        uint32_t per_mesh_size = per_model.per_mesh.size();
        for (uint32_t i = 0; i < per_mesh_size; ++i)
        {
            Model::MeshRange & mesh_range = per_model.model->getMeshRange(i);

            for (auto & per_material : per_model.per_mesh[i].per_material)
            {
                if (per_material.instances.empty()) continue;

                Material & material = per_material.material;

                globals->setPerEmissiveMeshBuffer(mesh_range.mesh_to_model);
                globals->updatePerEmissiveMeshBuffer();

                // if emissive is texture:
                // if (static_cast<bool>(material.emissive)) material.emissive->bind(0);

                uint32_t instances_count = uint32_t(per_material.instances.size());

                globals->device_context4->DrawIndexedInstanced(mesh_range.index_count,
                                                               instances_count,
                                                               mesh_range.index_offset,
                                                               mesh_range.vertex_offset,
                                                               rendered_instances);               
                rendered_instances += instances_count;
            }
        }
    }
}
} // namespace engine
