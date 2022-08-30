#include "scene.hpp"

namespace
{
constexpr float BACKGROUND[4] = {0.4f, 0.44f, 0.4f, 1.0f};
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

    globals->bind(camera, post_process.EV_100);

    window.bindViewport();
    
    bindRenderTarget();
    bindDepthBuffer();

    clearRenderTarget();
    clearDepthBuffer();

    mesh_system->render();
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
    dsb_desc.SampleDesc.Count = 1;
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
    dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
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
    texture_desc.SampleDesc.Count = 1;
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
    RTV_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    RTV_desc.Texture2D.MipSlice = 0;

    result = globals->device5->CreateRenderTargetView(HDR_texture.ptr(),
                                                      &RTV_desc,
                                                      HDR_RTV.reset());
    assert(result >= 0 && "CreateRenderTargetView");

    // create HDR shader resource view
    D3D11_SHADER_RESOURCE_VIEW_DESC SRV_desc;
    SRV_desc.Format = texture_desc.Format;
    SRV_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
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
} // namespace engine

