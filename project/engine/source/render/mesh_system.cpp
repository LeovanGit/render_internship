#include "mesh_system.hpp"

namespace engine
{
MeshSystem * MeshSystem::instance = nullptr;

void MeshSystem::init()
{
    if (!instance) instance = new MeshSystem();
    else spdlog::error("MeshSystem::init() was called twice!");
}

MeshSystem * MeshSystem::getInstance()
{
    return instance;
}

void MeshSystem::del()
{
    if (instance)
    {
        delete instance;
        instance = nullptr;
    }
    else spdlog::error("MeshSystem::del() was called twice!");
}

void MeshSystem::render()
{
    opaque_instances.render();
    emissive_instances.render();
}
} // namespace engine
