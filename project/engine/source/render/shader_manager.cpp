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

void ShaderManager::registerShader(const std::string & key,
                                   WCHAR * path,
                                   WCHAR * filename,
                                   D3D11_INPUT_ELEMENT_DESC input_desc[],
                                   uint32_t input_desc_size)
{
    shaders.try_emplace(key,
                        std::make_shared<Shader>(path,
                                                 filename,
                                                 input_desc,
                                                 input_desc_size));
}

void ShaderManager::useShader(const std::string & key)
{
    shaders.find(key)->second->bind();
}

std::shared_ptr<Shader> ShaderManager::getShader(const std::string & key)
{
    return shaders.find(key)->second;
}
} // namespace engine
