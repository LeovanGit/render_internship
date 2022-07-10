#ifndef TEXTURE_MANAGER_HPP
#define TEXTURE_MANAGER_HPP

#include "DDSTextureLoader11.h"

#include "globals.hpp"
#include "dx_res_ptr.hpp"
#include <d3d11.h>

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

    void initSampler();

    void loadNewTexture(WCHAR * texture_filename);

    DxResPtr<ID3D11ShaderResourceView> texture_view;
    DxResPtr<ID3D11SamplerState> sampler_state;

private:
    TextureManager() = default;
    ~TextureManager() = default;

    static TextureManager * instance;
};
} // namespace engine

#endif
