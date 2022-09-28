#include "smoke_emitter.hpp"

namespace
{
constexpr float PARTICLE_INITIAL_SIZE = 1.0f;
constexpr float PARTICLE_THICKNESS = 1.0f;
} // namespace

namespace engine
{
SmokeEmitter::SmokeEmitter(const glm::vec3 & position,
                           float radius,
                           const glm::vec3 & tint,
                           float spawn_rate,
                           float movement_speed,
                           float resize_speed,
                           float appear_speed,
                           float disappear_speed) :
                           position(position),
                           radius(radius),
                           tint(tint),
                           spawn_rate(spawn_rate),
                           movement_speed(movement_speed),
                           resize_speed(resize_speed),
                           appear_speed(appear_speed),
                           disappear_speed(disappear_speed),
                           start_time(std::chrono::steady_clock::now())
{}

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

float SmokeEmitter::randomFromRange(float min, float max)
{
    std::random_device random_device;
    std::mt19937 mersenne_random(random_device());
    
    std::uniform_real_distribution<float> distribution(min, max);
    return distribution(random_device);
}

void SmokeEmitter::spawnParticle()
{    
    glm::vec3 pos = glm::vec3(randomFromRange(position.x - radius, position.x + radius),
                              position.y,
                              randomFromRange(position.z - radius, position.z + radius));

    float angle = randomFromRange(0.0f, 2 * math::PI);
    
    particles.push_back(Particle(pos,
                                 glm::vec2(PARTICLE_INITIAL_SIZE),
                                 PARTICLE_THICKNESS,
                                 angle,
                                 glm::vec4(tint, 0.0f)));
}

void SmokeEmitter::update(float delta_time)
{
    if (spawnTimeElapsed()) spawnParticle();

    for (uint32_t i = 0; i != particles.size(); ++i)
    {
        particles[i].position.y += movement_speed * delta_time;
        particles[i].size += glm::vec2(resize_speed * delta_time);

        if (particles[i].is_disappears)
            particles[i].tint.w -= disappear_speed * delta_time;
        else
            particles[i].tint.w += appear_speed * delta_time;

        if (particles[i].tint.w >= 1.0f) particles[i].is_disappears = true;        
        else if (particles[i].tint.w <= 0)
        {
            particles.erase(particles.begin() + i);
            --i;
        }
    }
}

const std::vector<Particle> & SmokeEmitter::getParticles() const
{
    return particles;
}

} // namespace engine
