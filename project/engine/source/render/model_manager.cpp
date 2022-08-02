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

Model & ModelManager::getModel(const std::string & model_path)
{
    auto item = models.find(model_path);
    if (item != models.end()) return item->second;

    auto result = models.try_emplace(model_path, model_path);
    return result.first->second;
}

void ModelManager::bindModel(const std::string & key)
{
    models.find(key)->second.bind();
}

Model & ModelManager::getDefaultCube(const std::string & key)
{
    auto item = models.find(key);
    if (item != models.end()) return item->second;
    
    typedef Model::Vertex Vertex;

    // CREATE VERTEX BUFFER
    Vertex vertices[] =
    {
        // UV with a little offset to disable wrapping
        //     POSITION                           UV
        Vertex{{-1.0f, -1.0f, -1.0f}, {0.01f, 0.99f}},
        Vertex{{-1.0f,  1.0f, -1.0f}, {0.01f, 0.01f}},
        Vertex{{ 1.0f,  1.0f, -1.0f}, {0.99f, 0.01f}},
        Vertex{{ 1.0f, -1.0f, -1.0f}, {0.99f, 0.99f}},
        Vertex{{ 1.0f, -1.0f,  1.0f}, {0.01f, 0.99f}},
        Vertex{{ 1.0f,  1.0f,  1.0f}, {0.01f, 0.01f}},
        Vertex{{-1.0f,  1.0f,  1.0f}, {0.99f, 0.01f}},
        Vertex{{-1.0f, -1.0f,  1.0f}, {0.99f, 0.99f}},

        // additional vertices for texturing top and bot
        Vertex{{ 1.0f,  1.0f,  1.0f}, {0.99f, 0.99f}},
        Vertex{{-1.0f,  1.0f,  1.0f}, {0.01f, 0.99f}},
        Vertex{{ 1.0f, -1.0f,  1.0f}, {0.99f, 0.01f}},
        Vertex{{-1.0f, -1.0f,  1.0f}, {0.01f, 0.01f}},
    };
    
    // CREATE INDEX BUFFER
    int indices[] =
    {
        // front
        0, 1, 3,
        1, 2, 3,

        // right
        3, 2, 5,
        4, 3, 5,
        
        // top
        8, 2, 1,
        1, 9, 8,
        
        // back
        5, 6, 4,
        6, 7, 4,

        // left
        6, 1, 0,
        0, 7, 6,

        // bot
        0, 3, 10,
        0, 10, 7        
    };
    
    Model model(vertices, 12, indices, 36);
    auto result = models.try_emplace(key, model);

    return result.first->second;
}
} // namespace engine
