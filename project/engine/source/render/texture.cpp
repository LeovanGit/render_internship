#include "texture.hpp"

namespace engine
{
Texture::Texture(const std::string & texture_path)
{
    HRESULT result;
    Globals * globals = Globals::getInstance();

    std::wstring path = std::wstring(texture_path.begin(),
                                     texture_path.end());
    
    result = DirectX::CreateDDSTextureFromFile(globals->device5.ptr(),
                                               path.c_str(),
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
