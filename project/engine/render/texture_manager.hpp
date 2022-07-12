#ifndef TEXTURE_MANAGER_HPP
#define TEXTURE_MANAGER_HPP

#include "globals.hpp"
#include "dx_res_ptr.hpp"
#include <d3d11.h>
#include <vector>

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

    void registerTexture(const Texture & texture);

    void registerTexture(WCHAR * texture_filename);

    Texture & getTexture(int index);

private:
    TextureManager() = default;
    ~TextureManager() = default;

    static TextureManager * instance;

    std::vector<Texture> textures;
};
} // namespace engine

#endif
