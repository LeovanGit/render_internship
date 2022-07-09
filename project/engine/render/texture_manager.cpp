#include "texture_manager.hpp"
#include <d3d11_3.h>

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
