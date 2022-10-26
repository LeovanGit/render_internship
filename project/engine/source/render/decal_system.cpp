#include "decal_system.hpp"

#include "spdlog.h"

namespace
{
constexpr glm::vec2 DECAL_INIT_SIZE(1.0f, 1.0f);
} // namespace

namespace engine
{
DecalSystem * DecalSystem::instance = nullptr;

void DecalSystem::init()
{
    if (!instance) instance = new DecalSystem();
    else spdlog::error("DecalsSystem::init() was called twice!");
}

DecalSystem * DecalSystem::getInstance()
{
    return instance;
}

void DecalSystem::del()
{
    if (instance)
    {
        delete instance;
        instance = nullptr;
    }
    else spdlog::error("DecalsSystem::del() was called twice!");
}

void DecalSystem::addDecal(const glm::vec3 & position)
{
    glm::vec3 albedo(math::randomFromRange(0.0f, 1.0f),
                     math::randomFromRange(0.0f, 1.0f),
                     math::randomFromRange(0.0f, 1.0f));
    
    decals.push_back(Decal(position,
                           DECAL_INIT_SIZE,
                           albedo));
}

void DecalSystem::updateInstanceBuffer()
{
    if (decals.size() == 0) return;

    instance_buffer.init(decals.size());
    D3D11_MAPPED_SUBRESOURCE mapped = instance_buffer.map();
    GPUInstance * dst = static_cast<GPUInstance *>(mapped.pData);

    uint32_t copied_count = 0;
    for (auto & decal : decals)
    {
        dst[copied_count++] = GPUInstance(decal.position,
                                          decal.size,
                                          decal.albedo);
    }
    
    instance_buffer.unmap();
}

void DecalSystem::render()
{
    updateInstanceBuffer();

    if (instance_buffer.get_size() == 0) return;

    Globals * globals = Globals::getInstance();
    
    globals->device_context4->
        IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    globals->bindDefaultBlendState();
    globals->bindRasterizer(true);

    shader->bind();
    instance_buffer.bind(1);
    
    normals->bind(0);

    globals->device_context4->DrawInstanced(6,
                                            instance_buffer.get_size(),
                                            0,
                                            0);
}
} // namespace engine
