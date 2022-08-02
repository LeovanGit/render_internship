#ifndef TEXTURE_MANAGER_HPP
#define TEXTURE_MANAGER_HPP

#include <unordered_map>
#include <string>
#include "spdlog.h"

#include "texture.hpp"

namespace engine
{
// Singleton for texture resources
class TextureManager final
{
public:
    // deleted methods should be public for better error messages
    TextureManager(const TextureManager & other) = delete;
    void operator=(const TextureManager & other) = delete;

    static void init();

    static TextureManager * getInstance();

    static void del();

    Texture & getTexture(const std::string & texture_path);

    void bindTexture(const std::string & key);

private:
    TextureManager() = default;
    ~TextureManager() = default;

    static TextureManager * instance;

    std::unordered_map<std::string, Texture> textures;
};
} // namespace engine

#endif
