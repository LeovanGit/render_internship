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

    // CREATE SAMPLER
    D3D11_SAMPLER_DESC sampler_desc;
    ZeroMemory(&sampler_desc, sizeof(sampler_desc));

    sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc.MinLOD = 0;
    sampler_desc.MaxLOD = D3D11_FLOAT32_MAX; // unlimited mipmap levels

    result = globals->device5->CreateSamplerState(&sampler_desc,
                                                  sampler_state.reset());
    assert(result >= 0 && "CreateSamplerState");
}
} // namespace engine
