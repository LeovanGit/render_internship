#include "light_system.hpp"

namespace
{
constexpr int SHADOW_MAP_SIZE = 1024;
constexpr uint32_t shadow_cubemaps_count = 4;
} // namespace


namespace engine
{
LightSystem * LightSystem::instance = nullptr;

void LightSystem::init()
{
    if (!instance)
    {
        instance = new LightSystem();
        
        instance->initShadowMap(SHADOW_MAP_SIZE);
        instance->initSquareViewport(SHADOW_MAP_SIZE);
    }   
    else spdlog::error("LightSystem::init() was called twice!");
}

LightSystem * LightSystem::getInstance()
{
    return instance;
}

void LightSystem::del()
{
    if (instance)
    {
        delete instance;
        instance = nullptr;
    }
    else spdlog::error("LightSystem::del() was called twice!");
}

void LightSystem::addDirectionalLight(const glm::vec3 & direction,
                                      const glm::vec3 & radiance,
                                      float solid_angle)
{
    DirectionalLight directional_light(direction, radiance, solid_angle);
    
    directional_lights.push_back(directional_light);
}

void LightSystem::addPointLight(uint32_t transform_id,
                                const glm::vec3 & radiance,
                                float radius)
{
    PointLight point_light(transform_id, radiance, radius);

    point_lights.push_back(point_light);
}

glm::vec3 LightSystem::radianceFromIrradianceAtDistance(const glm::vec3 & irradiance,
                                                        float distance,
                                                        float radius)
{
    float sina = std::min(1.0f, radius / distance);
    float cosa = sqrtf(1.0f - sina * sina);
    float occupation = 1.0f - cosa;

    return irradiance / occupation;
}

void LightSystem::initSquareViewport(int size)
{
    ZeroMemory(&shadow_map_viewport, sizeof(shadow_map_viewport));

    shadow_map_viewport.TopLeftX = 0;
    shadow_map_viewport.TopLeftY = 0;
    shadow_map_viewport.Width = static_cast<float>(size);
    shadow_map_viewport.Height = static_cast<float>(size);
    shadow_map_viewport.MinDepth = 0.0f;
    shadow_map_viewport.MaxDepth = 1.0f;
}

void LightSystem::bindSquareViewport()
{
    Globals * globals = Globals::getInstance();
    
    globals->device_context4->RSSetViewports(1, &shadow_map_viewport);
}

void LightSystem::initShadowMap(int size)
{
    HRESULT result;
    Globals * globals = Globals::getInstance();

    D3D11_TEXTURE2D_DESC shadow_map_desc;
    ZeroMemory(&shadow_map_desc, sizeof(shadow_map_desc));
    shadow_map_desc.Width = size;
    shadow_map_desc.Height = size;
    shadow_map_desc.MipLevels = 1;
    shadow_map_desc.ArraySize = 6 * shadow_cubemaps_count;
    shadow_map_desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
    shadow_map_desc.SampleDesc.Count = 1;
    shadow_map_desc.SampleDesc.Quality = 0;
    shadow_map_desc.Usage = D3D11_USAGE_DEFAULT;
    shadow_map_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
    shadow_map_desc.CPUAccessFlags = 0; 
    shadow_map_desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

    result = globals->device5->CreateTexture2D(&shadow_map_desc,
                                               NULL,
                                               shadow_map.reset());
    assert(result >= 0 && "CreateTexture2D");

    D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc;
    ZeroMemory(&dsv_desc, sizeof(dsv_desc));
    dsv_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
    dsv_desc.Texture2DArray.ArraySize = 6 * shadow_cubemaps_count;
    dsv_desc.Texture2DArray.FirstArraySlice = 0;
    dsv_desc.Texture2DArray.MipSlice = 0;
        
    result = globals->device5->CreateDepthStencilView(shadow_map.ptr(),
                                                      &dsv_desc,
                                                      shadow_map_dsv.reset());
    assert(result >= 0 && "CreateDepthStencilView");

    D3D11_DEPTH_STENCIL_DESC dss_desc;
    ZeroMemory(&dss_desc, sizeof(dss_desc));
    dss_desc.DepthEnable = true;
    dss_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
    dss_desc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_GREATER_EQUAL;

    result = globals->device5->CreateDepthStencilState(&dss_desc,
                                                       shadow_map_dss.reset());
    assert(result >= 0 && "CreateDepthStencilState");

    D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
    ZeroMemory(&srv_desc, sizeof(srv_desc));
    srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
    srv_desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    srv_desc.TextureCubeArray.MipLevels = 1;
    srv_desc.TextureCubeArray.MostDetailedMip = 0;
    srv_desc.TextureCubeArray.First2DArrayFace = 0;
    srv_desc.TextureCubeArray.NumCubes = shadow_cubemaps_count;

    result = globals->device5->CreateShaderResourceView(shadow_map.ptr(),
                                                        &srv_desc,
                                                        shadow_map_srv.reset());
    assert(result >= 0 && "CreateShaderResourceView");
}

void LightSystem::clearShadowMap()
{
    Globals * globals = Globals::getInstance();
    
    // fill depth buffer with 0
    globals->device_context4->ClearDepthStencilView(shadow_map_dsv.ptr(),
                                                    D3D11_CLEAR_DEPTH |
                                                    D3D11_CLEAR_STENCIL,
                                                    0.0f, // reversed depth
                                                    0);
}

void LightSystem::bindShadowMap()
{
    Globals * globals = Globals::getInstance();

    globals->device_context4->OMSetRenderTargets(0,
                                                 nullptr,
                                                 shadow_map_dsv.ptr());
    
    globals->device_context4->OMSetDepthStencilState(shadow_map_dss.ptr(),
                                                     0);
}

void LightSystem::bindShadowMapSRV(int slot)
{
    Globals * globals = Globals::getInstance();
    
    globals->device_context4->PSSetShaderResources(slot,
                                                   1,
                                                   shadow_map_srv.get());
}

const std::vector<LightSystem::DirectionalLight> & LightSystem::getDirectionalLights() const
{
    return directional_lights;
}

const std::vector<LightSystem::PointLight> & LightSystem::getPointLights() const
{
    return point_lights;
}
} // namespace engine
