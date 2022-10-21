#include "grass_system.hpp"

namespace engine
{
GrassSystem * GrassSystem::instance = nullptr;

void GrassSystem::init()
{
    if (!instance) instance = new GrassSystem();
    else spdlog::error("GrassSystem::init() was called twice!");
}

GrassSystem * GrassSystem::getInstance()
{
    return instance;
}

void GrassSystem::del()
{
    if (instance)
    {
        delete instance;
        instance = nullptr;
    }
    else spdlog::error("ParticleSystem::del() was called twice!");
}

void GrassSystem::addGrassField(const GrassField & grass_field)
{
    grass_fields.push_back(grass_field);
}

void GrassSystem::updateInstanceBuffer()
{
    std::vector<Grass> instances;

    for (auto & grass_field : grass_fields)
    {
        auto & src = grass_field.getGrass();
        instances.insert(instances.end(), src.begin(), src.end());
    }
    if (instances.size() == 0) return;

    instance_buffer.init(instances.size());
    D3D11_MAPPED_SUBRESOURCE mapped = instance_buffer.map();
    GPUInstance * dst = static_cast<GPUInstance *>(mapped.pData);

    uint32_t copied_count = 0;
    for (auto & grass : instances)
    {
        dst[copied_count++] = GPUInstance(grass.position,
                                          grass.size);
    }    
    
    instance_buffer.unmap();
}

void GrassSystem::render()
{
    updateInstanceBuffer();

    if (instance_buffer.get_size() == 0) return;

    Globals * globals = Globals::getInstance();
    
    globals->device_context4->
        IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    //globals->bindA2CBlendState();
    globals->bindDefaultBlendState();
    globals->bindRasterizer(true);

    shader->bind();
    instance_buffer.bind(1);
    
    albedo->bind(0);
    opacity->bind(1);
    roughness->bind(2);
    normal->bind(3);
    ambient_occlusion->bind(8);
    translucency->bind(9);

    globals->device_context4->DrawInstanced(18,
                                            instance_buffer.get_size(),
                                            0,
                                            0);
}

void GrassSystem::renderWithoutMaterials(int cubemaps_count)
{
    updateInstanceBuffer();

    if (instance_buffer.get_size() == 0) return;

    Globals * globals = Globals::getInstance();
    
    globals->device_context4->
        IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    globals->bindDefaultBlendState();
    globals->bindRasterizer(true);

    shadow_shader->bind();
    instance_buffer.bind(1);

    opacity->bind(0);

    globals->setPerShadowMeshBuffer();
    globals->updatePerShadowMeshBuffer();

    for (int cubemap_index = 0; cubemap_index != cubemaps_count; ++cubemap_index)
    {
        globals->setPerShadowCubemapBuffer(cubemap_index);
        globals->updatePerShadowCubemapBuffer();
                    
        globals->device_context4->DrawInstanced(18,
                                                instance_buffer.get_size(),
                                                0,
                                                0);
    }
}
} // namespace engine
