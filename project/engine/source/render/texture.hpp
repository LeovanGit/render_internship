#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include "DDSTextureLoader11.h"
#include "globals.hpp"
#include "dx_res_ptr.hpp"
#include <d3d11.h>

namespace engine
{
class Texture
{
public:
    Texture(WCHAR * texture_filename);

    void activate();

    DxResPtr<ID3D11ShaderResourceView> texture_view;
private:
};
} // namespace engine

#endif
