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

std::shared_ptr<Texture> TextureManager::getTexture(const std::string & texture_path)
{
    auto item = textures.find(texture_path);
    if (item != textures.end()) return item->second;

    auto result = textures.try_emplace(texture_path,
                                       std::make_shared<Texture>(texture_path));
    return result.first->second;
}

void TextureManager::bindTexture(const std::string & key)
{
    textures.find(key)->second->bind();
}
} // namespace engine
