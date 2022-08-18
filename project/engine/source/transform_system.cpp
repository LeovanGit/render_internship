#include "transform_system.hpp"

namespace engine
{
TransformSystem * TransformSystem::instance = nullptr;

void TransformSystem::init()
{
    if (!instance) instance = new TransformSystem();
    else spdlog::error("TransformSystem::init() was called twice!");
}

TransformSystem * TransformSystem::getInstance()
{
    return instance;
}

void TransformSystem::del()
{
    if (instance)
    {
        delete instance;
        instance = nullptr;
    }
    else spdlog::error("TransformSystem::del() was called twice!");
}
} // namespace engine
