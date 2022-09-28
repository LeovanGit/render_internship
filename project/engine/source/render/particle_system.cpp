#include "particle_system.hpp"

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

void ParticleSystem::updateInstanceBuffer()
{
    uint32_t total_instances = 0;

    for (auto & smoke_emitter : smoke_emitters)
        total_instances += smoke_emitter.getParticles().size();

    if (total_instances == 0) return;

    instance_buffer.init(total_instances);
    D3D11_MAPPED_SUBRESOURCE mapped = instance_buffer.map();
    GPUInstance * dst = static_cast<GPUInstance *>(mapped.pData);

    uint32_t copied_count = 0;
    for (auto & smoke_emitter : smoke_emitters)
    {
        for (auto & particle : smoke_emitter.getParticles())
        {
            dst[copied_count++] = GPUInstance(particle.position,
                                              glm::vec3(particle.size,
                                                        particle.thickness),
                                              particle.angle,
                                              particle.tint);
        }
    }
    
    instance_buffer.unmap();    
}

void ParticleSystem::render(float delta_time)
{       
    for (auto & smoke_emitter : smoke_emitters)
        smoke_emitter.update(delta_time);
    
    updateInstanceBuffer();

    if (instance_buffer.get_size() == 0) return;

    Globals * globals = Globals::getInstance();

    globals->device_context4->
        IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    globals->bindBlendState(true);

    shader->bind();
    instance_buffer.bind(1);
    texture->bind(8);
    
    globals->device_context4->DrawInstanced(6,
                                            instance_buffer.get_size(),
                                            0,
                                            0);
}

} // namespace engine

