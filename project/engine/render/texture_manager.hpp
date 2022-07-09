#ifndef TEXTURE_MANAGER_HPP
#define TEXTURE_MANAGER_HPP

#include "DDSTextureLoader11.h"

#include "globals.hpp"
#include "dx_res_ptr.hpp"

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

    void loadNewTexture(WCHAR * texture_filename);

    DxResPtr<ID3D11ShaderResourceView> texture_view;

private:
    TextureManager() = default;
    ~TextureManager() = default;

    static TextureManager * instance;
};
} // namespace engine

#endif
