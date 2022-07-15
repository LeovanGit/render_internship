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
                                   const Shader & shader)
{
    shaders.emplace(key, shader);
}

void ShaderManager::registerShader(const std::string & key,
                                   WCHAR * path,
                                   WCHAR * filename,
                                   D3D11_INPUT_ELEMENT_DESC input_desc[])
{
    // will call copy constructor (construct temp and then copy):
    // shaders.emplace(key, Shader(path, filename, input_desc));

    // just construct inside:
    shaders.emplace(std::piecewise_construct,
                    std::forward_as_tuple(key),
                    std::forward_as_tuple(path, filename, input_desc));
}

void ShaderManager::useShader(const std::string & key)
{
    shaders.find(key)->second.activate();
}
} // namespace engine
