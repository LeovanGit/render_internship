#include "texture_manager.hpp"
#include <d3d11.h>
#include <d3d11_3.h>
#include <minwinbase.h>

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
    textures.push_back(texture);
}

Texture & TextureManager::getTexture(int index)
{
    return textures[index];
}
} // namespace engine
