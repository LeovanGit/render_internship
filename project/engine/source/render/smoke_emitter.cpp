#include "smoke_emitter.hpp"

namespace
{
constexpr float PARTICLE_INIT_SIZE = 1.0f;
constexpr float PARTICLE_INIT_THICKNESS = 3.0f; // contact fading range
} // namespace

namespace engine
{
SmokeEmitter::SmokeEmitter(const glm::vec3 & position,
                           float radius,
                           const glm::vec3 & tint,
                           float spawn_rate,
                           float movement_speed,
                           float resize_speed,
                           float life_speed,
                           float appear_lifetime_value) :
                           position(position),
                           radius(radius),
                           tint(tint),
                           spawn_rate(spawn_rate),
                           movement_speed(movement_speed),
                           resize_speed(resize_speed),
                           life_speed(life_speed),
                           appear_lifetime_value(appear_lifetime_value),
                           start_time(std::chrono::steady_clock::now())
{
    this->appear_speed = life_speed / appear_lifetime_value;
    this->disappear_speed = life_speed / (1.0f - appear_lifetime_value);
}

bool SmokeEmitter::spawnTimeElapsed()
{
    using namespace std::chrono;

    duration<float> elapsed_time = steady_clock::now() - start_time;

    if (elapsed_time.count() >= spawn_rate)
    {
        start_time = steady_clock::now();
        return true;
    }
    return false;    
}

void SmokeEmitter::spawnParticle()
{
    glm::vec3 pos = glm::vec3(math::randomFromRange(position.x - radius, position.x + radius),
                              position.y,
                              math::randomFromRange(position.z - radius, position.z + radius));

    float angle = math::randomFromRange(0.0f, 2 * math::PI);
    
    particles.push_back(Particle(pos,
                                 glm::vec2(PARTICLE_INIT_SIZE),
                                 PARTICLE_INIT_THICKNESS,
                                 angle,
                                 glm::vec4(tint, 0.0f)));
}

void SmokeEmitter::update(float delta_time)
{
    if (spawnTimeElapsed()) spawnParticle();

    for (uint32_t i = 0; i != particles.size(); ++i)
    {
        particles[i].lifetime += life_speed * delta_time;

        if (particles[i].lifetime >= 1.0f ||
            particles[i].tint.w < 0.0f)
        {
            particles.erase(particles.begin() + i);
            --i;
            continue;
        }

        particles[i].position.y += movement_speed * delta_time;
        particles[i].size += glm::vec2(resize_speed * delta_time);
        
        if (particles[i].lifetime < appear_lifetime_value)
            particles[i].tint.w += appear_speed * delta_time;
        else
            particles[i].tint.w -= disappear_speed * delta_time;
    }
}

const std::vector<Particle> & SmokeEmitter::getParticles() const
{
    return particles;
}

} // namespace engine
