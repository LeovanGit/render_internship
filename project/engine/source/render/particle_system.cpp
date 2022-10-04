#include "particle_system.hpp"

namespace
{
constexpr uint32_t MSAA_SAMPLES_COUNT = 4;
} // namespace

namespace engine
{
ParticleSystem * ParticleSystem::instance = nullptr;

void ParticleSystem::init()
{
    if (!instance) instance = new ParticleSystem();
    else spdlog::error("ParticleSystem::init() was called twice!");
}

ParticleSystem * ParticleSystem::getInstance()
{
    return instance;
}

void ParticleSystem::del()
{
    if (instance)
    {
        delete instance;
        instance = nullptr;
    }
    else spdlog::error("ParticleSystem::del() was called twice!");
}

void ParticleSystem::addSmokeEmitter(const SmokeEmitter & smoke_emitter)
{
    smoke_emitters.push_back(smoke_emitter);
}

void ParticleSystem::updateInstanceBuffer(const Camera & camera)
{
    std::vector<Particle> instances;

    for (auto & smoke_emitter : smoke_emitters)
    {
        auto & src = smoke_emitter.getParticles();
        instances.insert(instances.end(), src.begin(), src.end());
    }
    if (instances.size() == 0) return;

    std::sort(instances.begin(),
              instances.end(),
              [camera](const Particle & a, const Particle & b)
              {
                  float dist_1 = glm::length(a.position - camera.getPosition());
                  float dist_2 = glm::length(b.position - camera.getPosition());

                  return dist_1 > dist_2;
              });

    instance_buffer.init(instances.size());
    D3D11_MAPPED_SUBRESOURCE mapped = instance_buffer.map();
    GPUInstance * dst = static_cast<GPUInstance *>(mapped.pData);

    uint32_t copied_count = 0;
    for (auto & particle : instances)
    {
        dst[copied_count++] = GPUInstance(particle.position,
                                          glm::vec3(particle.size,
                                                    particle.thickness),
                                          particle.angle,
                                          particle.tint,
                                          particle.lifetime);
    }    
    
    instance_buffer.unmap();
}

void ParticleSystem::render(float delta_time,
                            const Camera & camera,
                            DxResPtr<ID3D11ShaderResourceView> depth_srv_not_ms)
{       
    for (auto & smoke_emitter : smoke_emitters)
        smoke_emitter.update(delta_time);
    
    updateInstanceBuffer(camera);

    if (instance_buffer.get_size() == 0) return;

    Globals * globals = Globals::getInstance();

    globals->device_context4->
        IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    globals->bindBlendState(true);

    shader->bind();
    instance_buffer.bind(1);
    
    lightmap_RLT->bind(8);
    lightmap_BotBF->bind(9);
    motion_vectors->bind(10);

    globals->device_context4->PSSetShaderResources(11,
                                                   1,
                                                   depth_srv_not_ms.get());
    
    globals->device_context4->DrawInstanced(6,
                                            instance_buffer.get_size(),
                                            0,
                                            0);
}
} // namespace engine

