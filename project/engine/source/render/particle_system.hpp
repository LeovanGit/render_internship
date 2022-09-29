#ifndef PARTICLE_SYSTEM_HPP
#define PARTICLE_SYSTEM_HPP

#include <algorithm>
#include "glm.hpp"

#include "smoke_emitter.hpp"
#include "vertex_buffer.hpp"
#include "camera.hpp"

namespace engine
{
class ParticleSystem final
{    
public:
    // deleted methods should be public for better error messages
    ParticleSystem(const ParticleSystem & other) = delete;
    void operator=(const ParticleSystem & other) = delete;
    
    static void init();

    static ParticleSystem * getInstance();
    
    static void del();

    void addSmokeEmitter(const SmokeEmitter & smoke_emitter);

    void updateInstanceBuffer(const Camera & camera);
    void render(float delta_time,
                const Camera & camera);

    std::shared_ptr<Shader> shader;
    std::shared_ptr<Texture> texture; // move it to Emitter class for different textures
    
private:
    ParticleSystem() = default;
    ~ParticleSystem() = default;

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
    
    static ParticleSystem * instance;

    std::vector<SmokeEmitter> smoke_emitters;

    VertexBuffer<GPUInstance> instance_buffer;
};
} // namespace engine

#endif
