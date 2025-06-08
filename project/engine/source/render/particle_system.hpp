#ifndef PARTICLE_SYSTEM_HPP
#define PARTICLE_SYSTEM_HPP

#include <algorithm>
#include "glm.hpp"

#include "smoke_emitter.hpp"
#include "vertex_buffer.hpp"
#include "camera.hpp"
#include "mesh_system.hpp"

namespace engine
{
class ParticleSystem final
{    
public:
    struct GPUSparkData
    {
        glm::vec3 position;
        float spawn_time;
        glm::vec3 init_velocity;
        float particle_padding_0;
    };
    
    // deleted methods should be public for better error messages
    ParticleSystem(const ParticleSystem & other) = delete;
    void operator=(const ParticleSystem & other) = delete;
    
    static void init();

    static ParticleSystem * getInstance();
    
    static void del();

    void initSparksBuffers();
    void bindSparksBuffers(bool to_compute_shader = false);
    void unbindSparksBuffers();

    void renderSparks(DxResPtr<ID3D11ShaderResourceView> depth_copy_srv,
                      DxResPtr<ID3D11ShaderResourceView> normals_copy_srv);
    void renderParticles(float delta_time,
                         const Camera & camera,
                         DxResPtr<ID3D11ShaderResourceView> depth_copy_srv);

    
    void addSmokeEmitter(const SmokeEmitter & smoke_emitter);

    void updateInstanceBuffer(const Camera & camera);

    // move them to Emitter class for different textures:
    std::shared_ptr<Shader> shader;
    std::shared_ptr<Texture> lightmap_RLT;
    std::shared_ptr<Texture> lightmap_BotBF;
    std::shared_ptr<Texture> motion_vectors;

    std::shared_ptr<Shader> spawn_sparks;
    std::shared_ptr<Shader> update_ring_buffer;
    std::shared_ptr<Shader> update_sparks;
    std::shared_ptr<Shader> render_sparks;
    std::shared_ptr<Texture> spark;
    
private:
    ParticleSystem() = default;
    ~ParticleSystem() = default;

    void initSparksDataBuffer();
    void initSparksRangeBuffer();
    void initSparksIndirectBuffer();
    void initSparksIndirectBufferCopy();

    void copySparksIndirectBuffer();

    void spawnSparks();
    void updateSparks(DxResPtr<ID3D11ShaderResourceView> depth_copy_srv,
                      DxResPtr<ID3D11ShaderResourceView> normals_copy_srv);
    void drawSparks(DxResPtr<ID3D11ShaderResourceView> depth_copy_srv);

    static ParticleSystem * instance;
    
    struct GPUInstance
    {
        GPUInstance(const glm::vec3 & posWS,
                    const glm::vec3 & size,
                    float angle,
                    const glm::vec4 & tint,
                    float lifetime) :
                    posWS(posWS),
                    size(size),
                    angle(angle),
                    tint(tint),
                    lifetime(lifetime)
        {}
        
        glm::vec3 posWS;
        glm::vec3 size;
        float angle;
        glm::vec4 tint;
        float lifetime;
    };

    VertexBuffer<GPUInstance> instance_buffer;
    
    std::vector<SmokeEmitter> smoke_emitters;

    DxResPtr<ID3D11Buffer> sparks_data;
    DxResPtr<ID3D11UnorderedAccessView> sparks_data_view;
    
    DxResPtr<ID3D11Buffer> sparks_range;
    DxResPtr<ID3D11UnorderedAccessView> sparks_range_view;

    DxResPtr<ID3D11Buffer> sparks_indirect_args;
    DxResPtr<ID3D11UnorderedAccessView> sparks_indirect_args_view;

    // because impossible to use D3D11_RESOURCE_MISC_BUFFER_STRUCTURED and
    // D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS in the same buffer
    DxResPtr<ID3D11Buffer> sparks_indirect_args_copy;
};
} // namespace engine

#endif
