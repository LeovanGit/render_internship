#include "engine.hpp"

namespace engine
{
void Engine::init()
{
    // INIT SINGLETONS
    Globals::init();
    ShaderManager::init(); // depends on Globals
    TextureManager::init(); // depends on Globals
}

void Engine::del()
{
    // destruct singletons in reverse order!
    engine::TextureManager::del();
    engine::ShaderManager::del();
    engine::Globals::del();
}
} // namespace engine
