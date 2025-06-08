#include "shader_manager.hpp"

namespace engine
{
ShaderManager * ShaderManager::instance = nullptr;

void ShaderManager::init()
{
    if (!instance) instance = new ShaderManager();
    else spdlog::error("ShaderManager::init() was called twice!");
}

ShaderManager * ShaderManager::getInstance()
{
    return instance;
}

void ShaderManager::del()
{
    if (instance)
    {
        delete instance;
        instance = nullptr;
    }
    else spdlog::error("ShaderManager::del() was called twice!");
}

std::shared_ptr<Shader> ShaderManager::getShader(const std::string shader_path,
                                                 D3D11_INPUT_ELEMENT_DESC input_desc[],
                                                 uint32_t input_desc_size,
                                                 bool vertex_shader,
                                                 bool geometry_shader,
                                                 bool fragment_shader,
                                                 bool compute_shader)
{
    auto item = shaders.find(shader_path);
    if (item != shaders.end()) return item->second;

    auto result = shaders.try_emplace(shader_path,
                                      std::make_shared<Shader>(shader_path,
                                                               input_desc,
                                                               input_desc_size,
                                                               vertex_shader,
                                                               geometry_shader,
                                                               fragment_shader,
                                                               compute_shader));
    return result.first->second;
}

void ShaderManager::bindShader(const std::string & key)
{
    shaders.find(key)->second->bind();
}
} // namespace engine
