#include "shader_manager.hpp"

namespace engine
{
ShaderManager * ShaderManager::instance = nullptr;

void ShaderManager::init()
{
    if (!instance) instance = new ShaderManager();
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
}

void ShaderManager::registerShader(const Shader & shader)
{
    shaders.push_back(shader);
}

void ShaderManager::registerShader(WCHAR * filename,
                                   D3D11_INPUT_ELEMENT_DESC input_desc[])
{
    Shader shader(filename, input_desc);
    shaders.push_back(shader);
}

void ShaderManager::useShader(int index)
{
    shaders[index].activate();
}
} // namespace engine
