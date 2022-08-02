#include "opaque_instances.hpp"

namespace engine
{
void OpaqueInstances::updateInstanceBuffers()
{
    uint32_t total_instances = 0;
    
    for (auto & per_model : per_model)
        for (auto & per_mesh : per_model.per_mesh)
            for (auto & per_material : per_mesh.per_material)
                total_instances += uint32_t(per_material.instances.size());

    if (total_instances == 0) return;

    instance_buffer.init(total_instances);
    D3D11_MAPPED_SUBRESOURCE mapped = instance_buffer.map();
    Instance * dst = static_cast<Instance *>(mapped.pData);

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
                    dst[copied_count++] = per_material.instances[i];
                }
            }
        }
    }

    instance_buffer.unmap();
}

void OpaqueInstances::render()
{
    if (instance_buffer.get_size() == 0) return;

    Globals * globals = Globals::getInstance();
    ShaderManager * shader_mgr = ShaderManager::getInstance();

    globals->device_context4->
        IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    
    shader_mgr->useShader("opaque");    
    instance_buffer.bind(1);

    uint32_t rendered_instances = 0;
    
    for (auto & per_model: per_model)
    {
        if (!static_cast<bool>(per_model.model)) continue;

        // bind vertex and index buffers
        per_model.model->bind();

        uint32_t per_mesh_size = per_model.per_mesh.size();
        for (uint32_t i = 0; i < per_mesh_size; ++i)
        {
            Model::MeshRange & mesh_range = per_model.model->getMeshRange(i);

            globals->setPerMeshBuffer(mesh_range.mesh_to_model);
            globals->updatePerMeshBuffer();

            for (auto & per_material : per_model.per_mesh[i].per_material)
            {
                if (per_material.instances.empty()) continue;

                Material & material = per_material.material;
                
                material.texture->bind();

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
