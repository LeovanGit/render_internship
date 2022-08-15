#include "engine.hpp"

namespace engine
{
void Engine::init()
{
    // INIT SINGLETONS
    Globals::init();
    ShaderManager::init(); // depends on Globals
    TextureManager::init(); // depends on Globals
    ModelManager::init();
    MeshSystem::init();
    LightSystem::init();
}

void Engine::del()
{
    // destruct singletons in reverse order!
    LightSystem::del();
    MeshSystem::del();
    ModelManager::del();
    TextureManager::del();
    ShaderManager::del();
    Globals::del();
}
} // namespace engine
