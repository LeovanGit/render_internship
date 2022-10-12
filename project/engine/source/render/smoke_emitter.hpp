#ifndef SMOKE_EMITTER_HPP
#define SMOKE_EMITTER_HPP

#include "glm.hpp"
#include <vector>
#include <chrono>

#include "particle.hpp"
#include "texture_manager.hpp"
#include "shader_manager.hpp"
#include "constants.hpp"
#include "random.hpp"

namespace engine
{
class SmokeEmitter
{
public:
    SmokeEmitter(const glm::vec3 & position,
                 float radius,
                 const glm::vec3 & tint,
                 float spawn_rate,
                 float movement_speed,
                 float resize_speed,
                 float life_speed,
                 float appear_lifetime_value);

    void update(float delta_time);
    
    glm::vec3 position;
    float radius;
    glm::vec3 tint; // this value copied to Particle::tint
    
    float spawn_rate; // in seconds
    float movement_speed;
    float resize_speed;

    const std::vector<Particle> & getParticles() const;
    
private:
    float life_speed;

    float appear_lifetime_value;
    float appear_speed;
    float disappear_speed;
    
    bool spawnTimeElapsed();

    void spawnParticle();
    
    std::vector<Particle> particles;

    std::chrono::steady_clock::time_point start_time;
};
} // namespace engine

#endif
