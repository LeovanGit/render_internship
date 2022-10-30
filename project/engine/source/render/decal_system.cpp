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

void DecalSystem::addDecal(uint32_t model_id,
                           uint32_t transform_id,
                           const glm::vec3 & posMS,
                           const glm::vec3 & forward,
                           const glm::vec3 & right,
                           const glm::vec3 & up)
{
    glm::vec3 albedo(math::randomFromRange(0.0f, 1.0f),
                     math::randomFromRange(0.0f, 1.0f),
                     math::randomFromRange(0.0f, 1.0f));
    
    decals.push_back(Decal(model_id,
                           transform_id,
                           posMS,
                           DECAL_INIT_SIZE,
                           albedo,
                           forward,
                           right,
                           up));
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
        TransformSystem * trans_sys = TransformSystem::getInstance();
        auto & transform = trans_sys->transforms[decal.transform_id];
        
        glm::vec4 pos_h = transform.toMat4() * glm::vec4(decal.posMS, 1.0f);
        glm::vec3 position = glm::vec3(pos_h) / pos_h.w;

        glm::mat4x4 model_matrix(decal.right.x, decal.right.y, decal.right.z, 0.0f,
                                 decal.up.x, decal.up.y, decal.up.z, 0.0f,
                                 decal.forward.x, decal.forward.y, decal.forward.z, 0.0f,
                                 position.x, position.y, position.z, 1.0f);
        
        dst[copied_count++] = GPUInstance(position,
                                          decal.size,
                                          decal.albedo,
                                          model_matrix,
                                          glm::inverse(model_matrix),
                                          decal.model_id);
    }
    
    instance_buffer.unmap();
}

void DecalSystem::render(DxResPtr<ID3D11ShaderResourceView> depth_srv,
                         DxResPtr<ID3D11ShaderResourceView> normals_srv,
                         DxResPtr<ID3D11ShaderResourceView> model_id_srv)
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
    globals->device_context4->PSSetShaderResources(3,
                                                   1,
                                                   model_id_srv.get());

    globals->device_context4->DrawInstanced(36,
                                            instance_buffer.get_size(),
                                            0,
                                            0);
}
} // namespace engine
