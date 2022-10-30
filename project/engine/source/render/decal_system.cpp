#include "decal_system.hpp"

#include "spdlog.h"

namespace
{
constexpr glm::vec2 DECAL_INIT_SIZE(2.0f);
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

void DecalSystem::addDecal(const glm::vec3 & position,
                           const glm::vec3 & forward,
                           const glm::vec3 & right,
                           const glm::vec3 & up)
{
    glm::vec3 albedo(math::randomFromRange(0.0f, 1.0f),
                     math::randomFromRange(0.0f, 1.0f),
                     math::randomFromRange(0.0f, 1.0f));
    
    glm::mat4x4 model(right.x, right.y, right.z, 0.0f,
                      up.x, up.y, up.z, 0.0f,
                      forward.x, forward.y, forward.z, 0.0f,
                      position.x, position.y, position.z, 1.0f);
    
    decals.push_back(Decal(position,
                           DECAL_INIT_SIZE,
                           albedo,
                           model));
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
                                          decal.albedo,
                                          decal.model,
                                          decal.model_inv);
    }
    
    instance_buffer.unmap();
}

void DecalSystem::render(DxResPtr<ID3D11ShaderResourceView> depth_srv,
                         DxResPtr<ID3D11ShaderResourceView> normals_srv)
{
    updateInstanceBuffer();

    if (instance_buffer.get_size() == 0) return;

    Globals * globals = Globals::getInstance();
    
    globals->device_context4->
        IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    globals->bindDefaultBlendState();
    globals->bindRasterizer(false);

    shader->bind();
    instance_buffer.bind(1);
    
    normals->bind(0);
    globals->device_context4->PSSetShaderResources(1,
                                                   1,
                                                   depth_srv.get());
    globals->device_context4->PSSetShaderResources(2,
                                                   1,
                                                   normals_srv.get());

    globals->device_context4->DrawInstanced(36,
                                            instance_buffer.get_size(),
                                            0,
                                            0);
}
} // namespace engine
