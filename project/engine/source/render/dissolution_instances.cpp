#include "dissolution_instances.hpp"

namespace engine
{
void DissolutionInstances::updateInstanceBuffers()
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
                        per_material.instances[i].spawn_time,
                        per_material.instances[i].animation_time);
                }
            }
        }
    }

    instance_buffer.unmap();
}

void DissolutionInstances::render()
{
    if (instance_buffer.get_size() == 0) return;

    Globals * globals = Globals::getInstance();
    LightSystem * light_sys = LightSystem::getInstance();

    globals->bindDissolveBlendState();
    
    globals->device_context4->
        IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    
    shader->bind();
    instance_buffer.bind(1);

    reflectance->bind(4);
    irradiance->bind(5);
    reflection->bind(6);

    dissolve->bind(13);

    light_sys->bindShadowMapSRV(7);
    
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

            for (auto & per_material : per_model.per_mesh[i].per_material)
            {
                if (per_material.instances.empty()) continue;

                Material & material = per_material.material;

                globals->bindRasterizer(material.is_double_sided);

                globals->setPerMeshBuffer(mesh_range.mesh_to_model,
                                          static_cast<bool>(material.albedo),
                                          static_cast<bool>(material.roughness),
                                          static_cast<bool>(material.metalness),
                                          static_cast<bool>(material.normal),
                                          material.is_directx_style_normal_map,
                                          material.albedo_default,
                                          material.roughness_default,
                                          material.metalness_default);
                globals->updatePerMeshBuffer();
                
                if (static_cast<bool>(material.albedo)) material.albedo->bind(0);
                if (static_cast<bool>(material.roughness)) material.roughness->bind(1);
                if (static_cast<bool>(material.metalness)) material.metalness->bind(2);
                if (static_cast<bool>(material.normal)) material.normal->bind(3);
                
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

void DissolutionInstances::renderWithoutMaterials(int cubemaps_count)
{
    if (instance_buffer.get_size() == 0) return;

    Globals * globals = Globals::getInstance();

    globals->bindDissolveBlendState();
    
    globals->device_context4->
        IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

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

            for (auto & per_material : per_model.per_mesh[i].per_material)
            {
                if (per_material.instances.empty()) continue;

                Material & material = per_material.material;

                globals->bindRasterizer(material.is_double_sided);

                globals->setPerShadowMeshBuffer(mesh_range.mesh_to_model);
                globals->updatePerShadowMeshBuffer();
                
                uint32_t instances_count = uint32_t(per_material.instances.size());

                for (int cubemap_index = 0; cubemap_index != cubemaps_count; ++cubemap_index)
                {
                    globals->setPerShadowCubemapBuffer(cubemap_index);
                    globals->updatePerShadowCubemapBuffer();
                    
                    globals->device_context4->DrawIndexedInstanced(mesh_range.index_count,
                                                                   instances_count,
                                                                   mesh_range.index_offset,
                                                                   mesh_range.vertex_offset,
                                                                   rendered_instances);
                }                
                rendered_instances += instances_count;
            }
        }
    }
}
} // namespace engine
