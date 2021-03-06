#include "texture_manager.hpp"

namespace engine
{
TextureManager * TextureManager::instance = nullptr;

void TextureManager::init()
{
    if (!instance) instance = new TextureManager();
    else spdlog::error("TextureManager::init() was called twice!");
}

TextureManager * TextureManager::getInstance()
{
    return instance;
}

void TextureManager::del()
{
    if (instance)
    {
        delete instance;
        instance = nullptr;
    }
    else spdlog::error("TextureManager::del() was called twice!");
}

void TextureManager::registerTexture(const std::string & key,
                                     const Texture & texture)
{
    textures.emplace(key, texture);
}

void TextureManager::registerTexture(const std::string & key,
                                     WCHAR * texture_filename)
{
    // will call copy constructor (construct temp and then copy):
    // textures.emplace(key, Shader(texture_filename));

    // just construct inside:
    textures.try_emplace(key, texture_filename);
}

void TextureManager::useTexture(const std::string & key)
{
    textures.find(key)->second.bind();
}

Texture & TextureManager::getTexture(const std::string & key)
{
    return textures.find(key)->second;
}
} // namespace engine
