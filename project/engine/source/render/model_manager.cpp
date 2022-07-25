#include "model_manager.hpp"

namespace engine
{
ModelManager * ModelManager::instance = nullptr;

void ModelManager::init()
{
    if (!instance) instance = new ModelManager();
    else spdlog::error("ModelManager::init() was called twice!");
}

ModelManager * ModelManager::getInstance()
{
    return instance;
}

void ModelManager::del()
{
    if (instance)
    {
        delete instance;
        instance = nullptr;
    }
    else spdlog::error("ModelManager::del() was called twice!");
}

void ModelManager::registerModel(const std::string & key,
                                 const Model & model)
{
    models.emplace(key, model);
}

void ModelManager::registerModel(const std::string & key,
                                 WCHAR * model_filename)
{
    models.try_emplace(key, model_filename);
}

void ModelManager::useModel(const std::string & key)
{
    models.find(key)->second.bind();
}
} // namespace engine
