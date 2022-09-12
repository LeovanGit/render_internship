#include "scene.hpp"

namespace
{
constexpr float BACKGROUND[4] = {0.4f, 0.44f, 0.4f, 1.0f};
constexpr int SHADOW_MAP_SIZE = 1024;
} // namespace

namespace engine
{
void Scene::init(const windows::Window & window)
{
    RECT client_size = window.getClientSize();
    int width = client_size.right - client_size.left;
    int height = client_size.bottom - client_size.top;

    initDepthBuffer(width, height);
    initRenderTarget(width, height);

    initShadowMap(SHADOW_MAP_SIZE);
    initSquareViewport(SHADOW_MAP_SIZE);
}

void Scene::renderFrame(windows::Window & window,
                        const Camera & camera,
                        engine::Postprocess & post_process) 
{
    Globals * globals = Globals::getInstance();
    TextureManager * tex_mgr = TextureManager::getInstance();
    ShaderManager * shader_mgr = ShaderManager::getInstance();
    ModelManager * model_mgr = ModelManager::getInstance();
    MeshSystem * mesh_system = MeshSystem::getInstance();
    LightSystem * light_system = LightSystem::getInstance();
    TransformSystem * trans_system = TransformSystem::getInstance();

    globals->setPerFrameBuffer(camera, post_process.EV_100);
    globals->updatePerFrameBuffer();
    
    // render shadow maps
    bindSquareViewport();

    bindShadowMap();
    clearShadowMap();

    globals->device_context4->OMSetRenderTargets(0,
                                                 nullptr,
                                                 shadow_map_dsv.ptr());

    mesh_system->renderDepthToCubemap();

    // render scene
    globals->bindSampler();

    window.bindViewport();
    
    bindRenderTarget();
    bindDepthBuffer();

    clearRenderTarget();
    clearDepthBuffer();
    
    mesh_system->render(shadow_map_srv);
    sky.render();

    post_process.resolve(HDR_SRV, window.getRenderTarget());

    window.switchBuffer();    
}

void Scene::initDepthBuffer(int width, int height)
{
    HRESULT result;
    Globals * globals = Globals::getInstance();

    // create depth buffer
    D3D11_TEXTURE2D_DESC dsb_desc;
    ZeroMemory(&dsb_desc, sizeof(dsb_desc));
    dsb_desc.Width = width;
    dsb_desc.Height = height;
    dsb_desc.MipLevels = 1;
    dsb_desc.ArraySize = 1;
    dsb_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsb_desc.SampleDesc.Count = 4;
    dsb_desc.SampleDesc.Quality = 0;
    dsb_desc.Usage = D3D11_USAGE_DEFAULT;
    dsb_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    dsb_desc.CPUAccessFlags = 0; 
    dsb_desc.MiscFlags = 0;

    result = globals->device5->CreateTexture2D(&dsb_desc,
                                               NULL,
                                               depth_stencil_buffer.reset());
    assert(result >= 0 && "CreateTexture2D");

    // create depth stencil view
    D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc;
    ZeroMemory(&dsv_desc, sizeof(dsv_desc));
    dsv_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
    dsv_desc.Texture2D.MipSlice = 0;

    result = globals->device5->CreateDepthStencilView(depth_stencil_buffer.ptr(),
                                                      &dsv_desc,
                                                      depth_stencil_view.reset());
    assert(result >= 0 && "CreateDepthStencilView");

    D3D11_DEPTH_STENCIL_DESC dss_desc;
    ZeroMemory(&dss_desc, sizeof(dss_desc));
    dss_desc.DepthEnable = true;
    dss_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
    dss_desc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_GREATER_EQUAL;

    result = globals->device5->CreateDepthStencilState(&dss_desc,
                                                       depth_stencil_state.reset());
    assert(result >= 0 && "CreateDepthStencilState");
}

void Scene::clearDepthBuffer()
{
    Globals * globals = Globals::getInstance();
    
    // fill depth buffer with 0
    globals->device_context4->ClearDepthStencilView(depth_stencil_view.ptr(),
                                                    D3D11_CLEAR_DEPTH |
                                                    D3D11_CLEAR_STENCIL,
                                                    0.0f, // reversed depth
                                                    0);
}

void Scene::bindDepthBuffer()
{
    Globals * globals = Globals::getInstance();

    globals->device_context4->OMSetDepthStencilState(depth_stencil_state.ptr(),
                                                     0);
}

void Scene::initRenderTarget(int width, int height)
{
    Globals * globals = Globals::getInstance();
    HRESULT result;

    // create texture
    D3D11_TEXTURE2D_DESC texture_desc;
    ZeroMemory(&texture_desc, sizeof(texture_desc));
    texture_desc.Width = width;
    texture_desc.Height = height;
    texture_desc.MipLevels = 1;
    texture_desc.ArraySize = 1;
    texture_desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    texture_desc.SampleDesc.Count = 4;
    texture_desc.Usage = D3D11_USAGE_DEFAULT;
    texture_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    texture_desc.CPUAccessFlags = 0;
    texture_desc.MiscFlags = 0;

    result = globals->device5->CreateTexture2D(&texture_desc,
                                               NULL,
                                               HDR_texture.reset());
    assert(result >= 0 && "CreateTexture2D");
    
    // create HDR render target
    D3D11_RENDER_TARGET_VIEW_DESC RTV_desc;
    RTV_desc.Format = texture_desc.Format;
    RTV_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
    RTV_desc.Texture2D.MipSlice = 0;

    result = globals->device5->CreateRenderTargetView(HDR_texture.ptr(),
                                                      &RTV_desc,
                                                      HDR_RTV.reset());
    assert(result >= 0 && "CreateRenderTargetView");

    // create HDR shader resource view
    D3D11_SHADER_RESOURCE_VIEW_DESC SRV_desc;
    SRV_desc.Format = texture_desc.Format;
    SRV_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
    SRV_desc.Texture2D.MostDetailedMip = 0;
    SRV_desc.Texture2D.MipLevels = 1;

    result = globals->device5->CreateShaderResourceView(HDR_texture.ptr(),
                                                        &SRV_desc,
                                                        HDR_SRV.reset());
    assert(result >= 0 && "CreateShaderResourceView");
}

void Scene::clearRenderTarget()
{
    Globals * globals = Globals::getInstance();

    globals->device_context4->ClearRenderTargetView(HDR_RTV.ptr(),
                                                    BACKGROUND);
}

void Scene::bindRenderTarget()
{
    Globals * globals = Globals::getInstance();

    // set render target
    globals->device_context4->OMSetRenderTargets(1,
                                                 HDR_RTV.get(),
                                                 depth_stencil_view.ptr());
}

void Scene::initShadowMap(int size)
{
    HRESULT result;
    Globals * globals = Globals::getInstance();

    D3D11_TEXTURE2D_DESC shadow_map_desc;
    ZeroMemory(&shadow_map_desc, sizeof(shadow_map_desc));
    shadow_map_desc.Width = size;
    shadow_map_desc.Height = size;
    shadow_map_desc.MipLevels = 1;
    shadow_map_desc.ArraySize = 6; // * cube_maps_count
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
    dsv_desc.Texture2DArray.ArraySize = 6; // * cube_maps_count
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
    srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
    srv_desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    srv_desc.TextureCube.MipLevels = 1;
    srv_desc.TextureCube.MostDetailedMip = 0;

    result = globals->device5->CreateShaderResourceView(shadow_map.ptr(),
                                                        &srv_desc,
                                                        shadow_map_srv.reset());
    assert(result >= 0 && "CreateShaderResourceView");
}

void Scene::clearShadowMap()
{
    Globals * globals = Globals::getInstance();
    
    // fill depth buffer with 0
    globals->device_context4->ClearDepthStencilView(shadow_map_dsv.ptr(),
                                                    D3D11_CLEAR_DEPTH |
                                                    D3D11_CLEAR_STENCIL,
                                                    0.0f, // reversed depth
                                                    0);
}

void Scene::bindShadowMap()
{
    Globals * globals = Globals::getInstance();

    globals->device_context4->OMSetDepthStencilState(shadow_map_dss.ptr(),
                                                     0);
}

void Scene::initSquareViewport(int size)
{
    ZeroMemory(&shadow_map_viewport, sizeof(shadow_map_viewport));

    shadow_map_viewport.TopLeftX = 0;
    shadow_map_viewport.TopLeftY = 0;
    shadow_map_viewport.Width = static_cast<float>(size);
    shadow_map_viewport.Height = static_cast<float>(size);
    shadow_map_viewport.MinDepth = 0.0f;
    shadow_map_viewport.MaxDepth = 1.0f;
}

void Scene::bindSquareViewport()
{
    Globals * globals = Globals::getInstance();

    globals->device_context4->RSSetViewports(1, &shadow_map_viewport);
}

} // namespace engine

