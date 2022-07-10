#include "texture_manager.hpp"
#include <d3d11.h>
#include <d3d11_3.h>
#include <minwinbase.h>

namespace engine
{
TextureManager * TextureManager::instance = nullptr;

void TextureManager::init()
{
    if (!instance) instance = new TextureManager();
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
}

void TextureManager::initSampler()
{
    Globals * globals = Globals::getInstance();    
    
    D3D11_SAMPLER_DESC sampler_desc;
    ZeroMemory(&sampler_desc, sizeof(sampler_desc));

    sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc.MinLOD = 0;
    sampler_desc.MaxLOD = D3D11_FLOAT32_MAX; // unlimited mipmap levels

    HRESULT result = 
        globals->device5->CreateSamplerState(&sampler_desc,
                                             sampler_state.reset());
    assert(result >= 0 && "CreateSamplerState");
}

void TextureManager::loadNewTexture(WCHAR * texture_filename)
{
    Globals * globals = Globals::getInstance();

    HRESULT result = 
        DirectX::CreateDDSTextureFromFile(globals->device5.ptr(),
                                          texture_filename,
                                          nullptr,
                                          texture_view.get());
    assert(result >= 0 && "CreateDDSTextureFromFile");
}
} // namespace engine
