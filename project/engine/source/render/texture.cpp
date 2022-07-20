#include "texture.hpp"

namespace engine
{
Texture::Texture(WCHAR * texture_filename)
{
    HRESULT result;
    Globals * globals = Globals::getInstance();

    result = DirectX::CreateDDSTextureFromFile(globals->device5.ptr(),
                                               texture_filename,
                                               nullptr,
                                               texture_view.get());
    assert(result >= 0 && "CreateDDSTextureFromFile");
}

void Texture::bind()
{
    Globals * globals = Globals::getInstance();

    // bind texture to fragment shader
    globals->device_context4->PSSetShaderResources(0,
                                                   1,
                                                   texture_view.get());
}
} // namespace engine
