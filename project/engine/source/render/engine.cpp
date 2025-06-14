#include "engine.hpp"

namespace engine
{
void Engine::init()
{    
    // INIT SINGLETONS
    Globals::init();
    ShaderManager::init();
    TextureManager::init();
    ModelManager::init();
    MeshSystem::init();
    LightSystem::init();
    TransformSystem::init();
    ParticleSystem::init();
    GrassSystem::init();
    DecalSystem::init();

    TimeSystem::init();
}

void Engine::del()
{
    // destruct singletons in reverse order!
    DecalSystem::del();
    GrassSystem::del();
    ParticleSystem::del();
    TransformSystem::del();
    LightSystem::del();
    MeshSystem::del();
    ModelManager::del();
    TextureManager::del();
    ShaderManager::del();
    Globals::del();
}
} // namespace engine
