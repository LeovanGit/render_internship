#include "scene.hpp"

namespace
{
constexpr float BACKGROUND[4] = {0.4f, 0.44f, 0.4f, 1.0f};
constexpr uint32_t REFLECTION_MIPS_COUNT = 8;
constexpr uint32_t SHADOW_CUBEMAPS_COUNT = 4;
constexpr int SHADOW_MAP_SIZE = 1024;
constexpr glm::vec<2, int> PARTICLES_ATLAS_SIZE(8, 8);
constexpr uint32_t MSAA_SAMPLES_COUNT = 4;
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
                        engine::Postprocess & post_process,
                        float delta_time) 
{
    Globals * globals = Globals::getInstance();

    RECT client_size = window.getClientSize();
    int width = client_size.right - client_size.left;
    int height = client_size.bottom - client_size.top;
    
    globals->setPerFrameBuffer(REFLECTION_MIPS_COUNT,
                               SHADOW_MAP_SIZE,
                               PARTICLES_ATLAS_SIZE,
                               MSAA_SAMPLES_COUNT,
                               glm::vec<2, int>(width, height));
    globals->updatePerFrameBuffer();

    globals->setPerViewBuffer(camera,
                              post_process.EV_100);
    globals->updatePerViewBuffer();
    
    globals->bindSamplers();

    renderShadows();
    renderSceneObjects(window);
    sky.render();
    renderParticles(delta_time, camera);
    
    post_process.resolve(hdr_srv, window.getRenderTarget());
    window.switchBuffer();

    unbindSRV(0);
    unbindSRV(7);
    unbindSRV(11);
}

void Scene::initDepthBuffer(int width, int height)
{
    initDepthBufferMS(width, height);
    initDepthBufferMSCopy(width, height);
    initDepthBufferNotMSCopy(width, height);
}

void Scene::clearDepthBuffer()
{
    Globals * globals = Globals::getInstance();
    
    // fill depth buffer with 0
    globals->device_context4->ClearDepthStencilView(depth_dsv.ptr(),
                                                    D3D11_CLEAR_DEPTH |
                                                    D3D11_CLEAR_STENCIL,
                                                    0.0f, // reversed depth
                                                    0);
}

void Scene::bindDepthBuffer()
{
    Globals * globals = Globals::getInstance();

    globals->device_context4->OMSetDepthStencilState(depth_dss.ptr(),
                                                     0);
}

void Scene::changeDepthBufferAccess(bool is_read_only)
{
    Globals * globals = Globals::getInstance();
    
    D3D11_DEPTH_STENCIL_DESC dss_desc;
    depth_dss.ptr()->GetDesc(&dss_desc);

    if (is_read_only) dss_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    else dss_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;

    HRESULT result = globals->device5->CreateDepthStencilState(&dss_desc,
                                                               depth_dss.reset());
    assert(result >= 0 && "CreateDepthStencilState");

    bindDepthBuffer();
}

void Scene::copyDepthBuffer()
{
    Globals * globals = Globals::getInstance();

    globals->device_context4->CopyResource(depth_texture_copy.ptr(),
                                           depth_texture.ptr());
    
    depth_copy_shader->bind();

    bindDepthBufferSRV(12);
    
    globals->device_context4->OMSetRenderTargets(1,
                                                 depth_rtv_not_ms.get(),
                                                 NULL);
    
    globals->device_context4->Draw(3, 0);
}

void Scene::bindDepthBufferSRV(uint32_t slot)
{
    Globals * globals = Globals::getInstance();

    globals->device_context4->PSSetShaderResources(slot,
                                                   1,
                                                   depth_srv.get());
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
    texture_desc.SampleDesc.Count = MSAA_SAMPLES_COUNT;
    texture_desc.Usage = D3D11_USAGE_DEFAULT;
    texture_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    texture_desc.CPUAccessFlags = 0;
    texture_desc.MiscFlags = 0;

    result = globals->device5->CreateTexture2D(&texture_desc,
                                               NULL,
                                               hdr_texture.reset());
    assert(result >= 0 && "CreateTexture2D");
    
    // create HDR render target
    D3D11_RENDER_TARGET_VIEW_DESC rtv_desc;
    ZeroMemory(&rtv_desc, sizeof(rtv_desc));
    rtv_desc.Format = texture_desc.Format;
    rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;

    result = globals->device5->CreateRenderTargetView(hdr_texture.ptr(),
                                                      &rtv_desc,
                                                      hdr_rtv.reset());
    assert(result >= 0 && "CreateRenderTargetView");

    // create HDR shader resource view
    D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
    ZeroMemory(&srv_desc, sizeof(srv_desc));
    srv_desc.Format = texture_desc.Format;
    srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;

    result = globals->device5->CreateShaderResourceView(hdr_texture.ptr(),
                                                        &srv_desc,
                                                        hdr_srv.reset());
    assert(result >= 0 && "CreateShaderResourceView");
}

void Scene::clearRenderTarget()
{
    Globals * globals = Globals::getInstance();

    globals->device_context4->ClearRenderTargetView(hdr_rtv.ptr(),
                                                    BACKGROUND);
}

void Scene::bindRenderTarget()
{
    Globals * globals = Globals::getInstance();

    globals->device_context4->OMSetRenderTargets(1,
                                                 hdr_rtv.get(),
                                                 depth_dsv.ptr());
}

void Scene::unbindSRV(int slot)
{    
    Globals * globals = Globals::getInstance();
    
    ID3D11ShaderResourceView * null_resource = nullptr;
    
    globals->device_context4->VSSetShaderResources(slot,
                                                   1,
                                                   &null_resource);

    globals->device_context4->GSSetShaderResources(slot,
                                                   1,
                                                   &null_resource);

    globals->device_context4->PSSetShaderResources(slot,
                                                   1,
                                                   &null_resource);
}

void Scene::unbindRTVs()
{
    Globals * globals = Globals::getInstance();

    // unbind all render targets
    globals->device_context4->OMSetRenderTargets(0,
                                                 nullptr,
                                                 nullptr);
}

void Scene::renderSceneObjects(windows::Window & window)
{
    MeshSystem * mesh_system = MeshSystem::getInstance();
    
    window.bindViewport();
    
    bindRenderTarget();
    bindDepthBuffer();

    clearRenderTarget();
    clearDepthBuffer();
    
    mesh_system->render();
}

void Scene::renderShadows()
{
    Globals * globals = Globals::getInstance();
    LightSystem * light_system = LightSystem::getInstance();
    MeshSystem * mesh_system = MeshSystem::getInstance();

    globals->bindDefaultBlendState();
    
    light_system->bindSquareViewport();

    light_system->bindShadowMap();
    light_system->clearShadowMap();

    mesh_system->renderShadowCubeMaps(SHADOW_CUBEMAPS_COUNT);
}

void Scene::renderParticles(float delta_time,
                            const Camera & camera)
{
    Globals * globals = Globals::getInstance();
    engine::ParticleSystem * particle_sys = engine::ParticleSystem::getInstance();
    
    copyDepthBuffer();
    
    bindRenderTarget();
    
    changeDepthBufferAccess(true);
    
    particle_sys->render(delta_time, camera, depth_srv_not_ms);

    changeDepthBufferAccess(false);
}

void Scene::initDepthBufferMS(int width, int height)
{
    HRESULT result;
    Globals * globals = Globals::getInstance();

    D3D11_TEXTURE2D_DESC texture_desc;
    ZeroMemory(&texture_desc, sizeof(texture_desc));
    texture_desc.Width = width;
    texture_desc.Height = height;
    texture_desc.MipLevels = 1;
    texture_desc.ArraySize = 1;
    texture_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    texture_desc.SampleDesc.Count = MSAA_SAMPLES_COUNT;
    texture_desc.SampleDesc.Quality = 0;
    texture_desc.Usage = D3D11_USAGE_DEFAULT;
    texture_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    texture_desc.CPUAccessFlags = 0; 
    texture_desc.MiscFlags = 0;

    result = globals->device5->CreateTexture2D(&texture_desc,
                                               NULL,
                                               depth_texture.reset());
    assert(result >= 0 && "CreateTexture2D");

    D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc;
    ZeroMemory(&dsv_desc, sizeof(dsv_desc));
    dsv_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;

    result = globals->device5->CreateDepthStencilView(depth_texture.ptr(),
                                                      &dsv_desc,
                                                      depth_dsv.reset());
    assert(result >= 0 && "CreateDepthStencilView");

    D3D11_DEPTH_STENCIL_DESC dss_desc;
    ZeroMemory(&dss_desc, sizeof(dss_desc));
    dss_desc.DepthEnable = true;
    dss_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
    dss_desc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_GREATER_EQUAL;

    result = globals->device5->CreateDepthStencilState(&dss_desc,
                                                       depth_dss.reset());
    assert(result >= 0 && "CreateDepthStencilState");
}

void Scene::initDepthBufferMSCopy(int width, int height)
{
    HRESULT result;
    Globals * globals = Globals::getInstance();

    D3D11_TEXTURE2D_DESC texture_desc;
    depth_texture.ptr()->GetDesc(&texture_desc);
    texture_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    texture_desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
    result = globals->device5->CreateTexture2D(&texture_desc,
                                               NULL,
                                               depth_texture_copy.reset());
    assert(result >= 0 && "CreateTexture2D");
    
    D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
    ZeroMemory(&srv_desc, sizeof(srv_desc));
    srv_desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;

    result = globals->device5->CreateShaderResourceView(depth_texture_copy.ptr(),
                                                        &srv_desc,
                                                        depth_srv.reset());
    assert(result >= 0 && "CreateShaderResourceView");
}

void Scene::initDepthBufferNotMSCopy(int width, int height)
{
    Globals * globals = Globals::getInstance();
    HRESULT result;

    D3D11_TEXTURE2D_DESC dst_desc;
    ZeroMemory(&dst_desc, sizeof(dst_desc));
    dst_desc.Width = width;
    dst_desc.Height = height;
    dst_desc.MipLevels = 1;
    dst_desc.ArraySize = 1;
    dst_desc.Format = DXGI_FORMAT_R32_TYPELESS;
    dst_desc.SampleDesc.Count = 1;
    dst_desc.SampleDesc.Quality = 0;
    dst_desc.Usage = D3D11_USAGE_DEFAULT;
    dst_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    dst_desc.CPUAccessFlags = 0; 
    dst_desc.MiscFlags = 0;

    result = globals->device5->CreateTexture2D(&dst_desc,
                                               NULL,
                                               depth_texture_not_ms.reset());
    assert(result >= 0 && "CreateTexture2D");
    
    D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
    ZeroMemory(&srv_desc, sizeof(srv_desc));
    srv_desc.Format = DXGI_FORMAT_R32_FLOAT;
    srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srv_desc.Texture2D.MipLevels = 1;
    srv_desc.Texture2D.MostDetailedMip = 0;

    result = globals->
                 device5->
                     CreateShaderResourceView(depth_texture_not_ms.ptr(),
                                              &srv_desc,
                                              depth_srv_not_ms.reset());
    assert(result >= 0 && "CreateShaderResourceView");

    D3D11_RENDER_TARGET_VIEW_DESC rtv_desc;
    ZeroMemory(&rtv_desc, sizeof(rtv_desc));
    rtv_desc.Format = DXGI_FORMAT_R32_FLOAT;
    rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    rtv_desc.Texture2D.MipSlice = 0;

    result = globals->device5->CreateRenderTargetView(depth_texture_not_ms.ptr(),
                                                      &rtv_desc,
                                                      depth_rtv_not_ms.reset());
    assert(result >= 0 && "CreateRenderTargetView");
}
} // namespace engine

