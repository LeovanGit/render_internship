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

    std::vector<Instance> instances;
    
    for (auto & per_model : per_model)
    {
        for (auto & per_mesh : per_model.per_mesh)
        {
            for (auto & per_material : per_mesh.per_material)
            {
                instances.insert(instances.end(),
                                 per_material.instances.begin(),
                                 per_material.instances.end());
            }
        }
    }

    instance_buffer.init(instances.data(),
                         total_instances,
                         BufferType::INSTANCE);
}

void OpaqueInstances::render()
{
    if (instance_buffer.get_size() == 0) return;

    Globals * globals = Globals::getInstance();
    ShaderManager * shader_mgr = ShaderManager::getInstance();

    globals->device_context4->
        IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    
    shader_mgr->useShader("opaque");    
    instance_buffer.bind(1, 1);

    uint32_t rendered_instances = 0;
    
    for (auto & per_model: per_model)
    {
        if (!per_model.model) continue;

        // bind vertex and index buffers
        per_model.model->bind();

        uint32_t per_mesh_size = per_model.per_mesh.size();
        for (uint32_t i = 0; i < per_mesh_size; ++i)
        {
            Model::MeshRange & mesh_range = per_model.model->get_mesh(i);

            for (auto & per_material : per_model.per_mesh[i].per_material)
            {
                if (per_material.instances.empty()) continue;

                Material & material = per_material.material;

                // ... update shader local uniform buffer

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
