#include "texture_manager.hpp"

namespace engine
{
TextureManager * TextureManager::instance = nullptr;

void TextureManager::init()
{
    if (!instance) instance = new TextureManager();
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
}

void TextureManager::registerTexture(const Texture & texture)
{
    textures.push_back(texture);
}

void TextureManager::registerTexture(WCHAR * texture_filename)
{
    Texture texture(texture_filename);
    textures.push_back(std::move(texture));
}

Texture & TextureManager::getTexture(int index)
{
    return textures[index];
}
} // namespace engine
