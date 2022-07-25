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
}

void Engine::del()
{
    // destruct singletons in reverse order!
    ModelManager::del();
    TextureManager::del();
    ShaderManager::del();
    Globals::del();
}
} // namespace engine
