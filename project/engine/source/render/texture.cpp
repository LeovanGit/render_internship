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

void Texture::bind(uint32_t slot)
{
    Globals * globals = Globals::getInstance();

    globals->device_context4->PSSetShaderResources(slot,
                                                   1,
                                                   texture_view.get());
}
} // namespace engine
