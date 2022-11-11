#include "renderer.hpp"

namespace
{
constexpr float BACKGROUND[4] = {0.0f, 0.0f, 0.0f, 1.0f};
constexpr uint32_t REFLECTION_MIPS_COUNT = 8;
constexpr uint32_t SHADOW_CUBEMAPS_COUNT = 4;
constexpr int SHADOW_MAP_SIZE = 1024;
constexpr glm::vec<2, int> PARTICLES_ATLAS_SIZE(8, 8);
constexpr uint32_t MSAA_SAMPLES_COUNT = 1;
} // namespace

namespace engine
{
void Renderer::init(const windows::Window & window)
{
    RECT client_size = window.getClientSize();
    int width = client_size.right - client_size.left;
    int height = client_size.bottom - client_size.top;

    initGBuffer(width, height);
    initRenderTarget(width, height);
}

void Renderer::renderFrame(windows::Window & window,
                           const Camera & camera,
                           engine::Postprocess & post_process,
                           float delta_time) 
{
    Globals * globals = Globals::getInstance();
    MeshSystem * mesh_sys = MeshSystem::getInstance();

    RECT client_size = window.getClientSize();
    int width = client_size.right - client_size.left;
    int height = client_size.bottom - client_size.top;
    
    globals->setPerFrameBuffer(REFLECTION_MIPS_COUNT,
                               SHADOW_MAP_SIZE,
                               PARTICLES_ATLAS_SIZE,
                               glm::vec<2, int>(width, height),
                               delta_time);
    globals->updatePerFrameBuffer();

    globals->setPerViewBuffer(camera,
                              post_process.EV_100);
    globals->updatePerViewBuffer();
    
    globals->bindSamplers();

    renderShadows();

    setStencilTest(false);    
    bindDepthBuffer();
    clearDepthBuffer();
    
    bindGBufferRTV();
    clearGBuffer();
    
    renderSceneObjects(window);
    renderGrass();
    renderDecals();

    unbindRTVs();

    setStencilTest(true);
    deferredShading();

    disableStencilTest();
    sky.render();
    mesh_sys->renderLights();
    
    renderParticles(delta_time, camera);
    
    post_process.resolve(hdr_srv, window.getRenderTarget());
    window.switchBuffer();

    unbindSRVs();
}

void Renderer::initDepthBuffer(int width, int height)
{
    initDepthBufferMain(width, height);
    initDepthBufferCopy(width, height);
}

void Renderer::clearDepthBuffer()
{
    Globals * globals = Globals::getInstance();
    
    // fill depth buffer with 0
    globals->device_context4->ClearDepthStencilView(depth_dsv.ptr(),
                                                    D3D11_CLEAR_DEPTH |
                                                    D3D11_CLEAR_STENCIL,
                                                    0.0f, // reversed depth
                                                    0);
}

void Renderer::bindDepthBuffer()
{
    Globals * globals = Globals::getInstance();

    globals->device_context4->OMSetDepthStencilState(depth_dss.ptr(),
                                                     1);
}

void Renderer::changeDepthBufferAccess(bool is_read_only)
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

void Renderer::copyDepthBuffer()
{
    Globals * globals = Globals::getInstance();

    globals->device_context4->CopyResource(depth_copy.ptr(),
                                           depth.ptr());
}

void Renderer::fillDepthBufferFromCopy()
{
    Globals * globals = Globals::getInstance();

    globals->device_context4->CopyResource(depth.ptr(),
                                           depth_copy.ptr());
}

void Renderer::setStencilTest(bool is_read_only)
{
    Globals * globals = Globals::getInstance();
    
    D3D11_DEPTH_STENCIL_DESC dss_desc;
    depth_dss->GetDesc(&dss_desc);
    dss_desc.StencilEnable = true;
    dss_desc.StencilReadMask = 0xFF;
    dss_desc.StencilWriteMask = 0xFF;

    if (is_read_only)
    {
        dss_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        dss_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
        dss_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        dss_desc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
    }
    else
    {
        dss_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_REPLACE;
        dss_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_REPLACE;
        dss_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
        dss_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    }

    dss_desc.BackFace = dss_desc.FrontFace;

    HRESULT result = globals->device5->CreateDepthStencilState(&dss_desc,
                                                               depth_dss.reset());
    assert(result >= 0 && "CreateDepthStencilState");

    bindDepthBuffer();
}

void Renderer::disableStencilTest()
{
    Globals * globals = Globals::getInstance();
    
    D3D11_DEPTH_STENCIL_DESC dss_desc;
    depth_dss->GetDesc(&dss_desc);
    dss_desc.StencilEnable = false;

    HRESULT result = globals->device5->CreateDepthStencilState(&dss_desc,
                                                               depth_dss.reset());
    assert(result >= 0 && "CreateDepthStencilState");

    bindDepthBuffer();
}

void Renderer::initRenderTarget(int width, int height)
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
    rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    rtv_desc.Texture2D.MipSlice = 0;

    result = globals->device5->CreateRenderTargetView(hdr_texture.ptr(),
                                                      &rtv_desc,
                                                      hdr_rtv.reset());
    assert(result >= 0 && "CreateRenderTargetView");

    // create HDR shader resource view
    D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
    ZeroMemory(&srv_desc, sizeof(srv_desc));
    srv_desc.Format = texture_desc.Format;
    srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srv_desc.Texture2D.MostDetailedMip = 0;
    srv_desc.Texture2D.MipLevels = 1;

    result = globals->device5->CreateShaderResourceView(hdr_texture.ptr(),
                                                        &srv_desc,
                                                        hdr_srv.reset());
    assert(result >= 0 && "CreateShaderResourceView");
}

void Renderer::clearRenderTarget()
{
    Globals * globals = Globals::getInstance();

    globals->device_context4->ClearRenderTargetView(hdr_rtv.ptr(),
                                                    BACKGROUND);
}

void Renderer::bindRenderTarget(bool bind_depth_buffer)
{
    Globals * globals = Globals::getInstance();

    if (bind_depth_buffer)
    {
        globals->device_context4->OMSetRenderTargets(1,
                                                     hdr_rtv.get(),
                                                     depth_dsv.ptr());
    }
    else
    {
        globals->device_context4->OMSetRenderTargets(1,
                                                     hdr_rtv.get(),
                                                     NULL);   
    }
}

void Renderer::initGBuffer(int width, int height)
{
    initDepthBuffer(width, height);
    initNormalsTexture(width, height);
    initAlbedoTexture(width, height);
    initRoughnessMetalnessTexture(width, height);
    initEmissiveTexture(width, height);
    initModelIDTexture(width, height);

    initNormalsCopy(width, height);
}

void Renderer::bindGBufferSRV()
{
    Globals * globals = Globals::getInstance();

    globals->device_context4->PSSetShaderResources(0,
                                                   1,
                                                   normals_srv.get());

    globals->device_context4->PSSetShaderResources(1,
                                                   1,
                                                   albedo_srv.get());

    globals->device_context4->PSSetShaderResources(2,
                                                   1,
                                                   roughness_metalness_srv.get());

    globals->device_context4->PSSetShaderResources(3,
                                                   1,
                                                   emissive_ao_srv.get());

    globals->device_context4->PSSetShaderResources(4,
                                                   1,
                                                   model_id_srv.get());
}

void Renderer::bindGBufferRTV(bool bind_depth_buffer,
                              bool bind_model_id)
{
    Globals * globals = Globals::getInstance();

    std::vector<ID3D11RenderTargetView *> render_targets;

    render_targets.push_back(normals_rtv.ptr());
    render_targets.push_back(albedo_rtv.ptr());
    render_targets.push_back(roughness_metalness_rtv.ptr());
    render_targets.push_back(emissive_ao_rtv.ptr());
    if (bind_model_id) render_targets.push_back(model_id_rtv.ptr());

    if (bind_depth_buffer)
    {
        globals->device_context4->OMSetRenderTargets(render_targets.size(),
                                                     render_targets.data(),
                                                     depth_dsv.ptr());
    }
    else
    {
        globals->device_context4->OMSetRenderTargets(render_targets.size(),
                                                     render_targets.data(),
                                                     NULL);
    }
}

void Renderer::clearGBuffer()
{
    Globals * globals = Globals::getInstance();
    
    globals->device_context4->ClearRenderTargetView(normals_rtv.ptr(),
                                                    BACKGROUND);

    globals->device_context4->ClearRenderTargetView(albedo_rtv.ptr(),
                                                    BACKGROUND);

    globals->device_context4->ClearRenderTargetView(roughness_metalness_rtv.ptr(),
                                                    BACKGROUND);

    globals->device_context4->ClearRenderTargetView(emissive_ao_rtv.ptr(),
                                                    BACKGROUND);

    globals->device_context4->ClearRenderTargetView(model_id_rtv.ptr(),
                                                    BACKGROUND);
}

void Renderer::unbindSRV(int slot)
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

void Renderer::unbindSRVs()
{
    unbindSRV(0);
    unbindSRV(1);
    unbindSRV(2);
    unbindSRV(3);
    unbindSRV(4);
    unbindSRV(7);
    unbindSRV(11);
}

void Renderer::unbindRTVs()
{
    Globals * globals = Globals::getInstance();

    // unbind all render targets
    globals->device_context4->OMSetRenderTargets(0,
                                                 nullptr,
                                                 nullptr);
}

void Renderer::renderSceneObjects(windows::Window & window)
{
    MeshSystem * mesh_system = MeshSystem::getInstance();
    
    window.bindViewport();
        
    mesh_system->render();
}

void Renderer::renderShadows()
{
    Globals * globals = Globals::getInstance();
    LightSystem * light_system = LightSystem::getInstance();
    MeshSystem * mesh_system = MeshSystem::getInstance();
    GrassSystem * grass_system = GrassSystem::getInstance();
    
    globals->bindDefaultBlendState();
    
    light_system->bindSquareViewport();

    light_system->bindShadowMap();
    light_system->clearShadowMap();

    mesh_system->renderShadowCubeMaps(SHADOW_CUBEMAPS_COUNT);
    grass_system->renderWithoutMaterials(SHADOW_CUBEMAPS_COUNT);
}

void Renderer::renderParticles(float delta_time,
                               const Camera & camera)
{
    Globals * globals = Globals::getInstance();
    engine::ParticleSystem * particle_sys = engine::ParticleSystem::getInstance();

    copyDepthBuffer();
    copyNormals();
    
    bindRenderTarget();
    
    changeDepthBufferAccess(true);    

    particle_sys->renderParticles(delta_time, camera, depth_copy_srv);
    particle_sys->renderSparks(depth_copy_srv, normals_copy_srv);

    changeDepthBufferAccess(false);
}

void Renderer::renderGrass()
{
    engine::GrassSystem * grass_system = engine::GrassSystem::getInstance();

    grass_system->render();
}

void Renderer::renderDecals()
{
    DecalSystem * decal_sys = DecalSystem::getInstance();

    copyDepthBuffer();
    copyNormals();

    bindGBufferRTV(false, false);
    
    decal_sys->render(depth_copy_srv,
                      normals_copy_srv,
                      model_id_srv);
}

void Renderer::initDepthBufferMain(int width, int height)
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
                                               depth.reset());
    assert(result >= 0 && "CreateTexture2D");

    D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc;
    ZeroMemory(&dsv_desc, sizeof(dsv_desc));
    dsv_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

    result = globals->device5->CreateDepthStencilView(depth.ptr(),
                                                      &dsv_desc,
                                                      depth_dsv.reset());
    assert(result >= 0 && "CreateDepthStencilView");

    D3D11_DEPTH_STENCIL_DESC dss_desc;
    ZeroMemory(&dss_desc, sizeof(dss_desc));
    dss_desc.DepthEnable = true;
    dss_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dss_desc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;

    result = globals->device5->CreateDepthStencilState(&dss_desc,
                                                       depth_dss.reset());
    assert(result >= 0 && "CreateDepthStencilState");
}

void Renderer::initDepthBufferCopy(int width, int height)
{
    HRESULT result;
    Globals * globals = Globals::getInstance();

    D3D11_TEXTURE2D_DESC texture_desc;
    depth.ptr()->GetDesc(&texture_desc);
    texture_desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
    texture_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
 
    result = globals->device5->CreateTexture2D(&texture_desc,
                                               NULL,
                                               depth_copy.reset());
    assert(result >= 0 && "CreateTexture2D");
    
    D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
    ZeroMemory(&srv_desc, sizeof(srv_desc));
    srv_desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srv_desc.Texture2D.MipLevels = 1;
    srv_desc.Texture2D.MostDetailedMip = 0;

    result = globals->device5->CreateShaderResourceView(depth_copy.ptr(),
                                                        &srv_desc,
                                                        depth_copy_srv.reset());
    assert(result >= 0 && "CreateShaderResourceView");
}

// .rg - normals
// .ba - geometry normals
// using octahedron mapping
void Renderer::initNormalsTexture(int width, int height)
{
    Globals * globals = Globals::getInstance();
    HRESULT result;
    
    D3D11_TEXTURE2D_DESC texture_desc;
    ZeroMemory(&texture_desc, sizeof(texture_desc));
    texture_desc.Width = width;
    texture_desc.Height = height;
    texture_desc.MipLevels = 1;
    texture_desc.ArraySize = 1;
    texture_desc.Format = DXGI_FORMAT_R16G16B16A16_SNORM;
    texture_desc.SampleDesc.Count = MSAA_SAMPLES_COUNT;
    texture_desc.Usage = D3D11_USAGE_DEFAULT;
    texture_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    texture_desc.CPUAccessFlags = 0;
    texture_desc.MiscFlags = 0;

    result = globals->device5->CreateTexture2D(&texture_desc,
                                               NULL,
                                               normals.reset());
    assert(result >= 0 && "CreateTexture2D");
    
    D3D11_RENDER_TARGET_VIEW_DESC rtv_desc;
    ZeroMemory(&rtv_desc, sizeof(rtv_desc));
    rtv_desc.Format = texture_desc.Format;
    rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    rtv_desc.Texture2D.MipSlice = 0;

    result = globals->device5->CreateRenderTargetView(normals.ptr(),
                                                      &rtv_desc,
                                                      normals_rtv.reset());
    assert(result >= 0 && "CreateRenderTargetView");

    D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
    ZeroMemory(&srv_desc, sizeof(srv_desc));
    srv_desc.Format = texture_desc.Format;
    srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srv_desc.Texture2D.MostDetailedMip = 0;
    srv_desc.Texture2D.MipLevels = 1;

    result = globals->device5->CreateShaderResourceView(normals.ptr(),
                                                        &srv_desc,
                                                        normals_srv.reset());
    assert(result >= 0 && "CreateShaderResourceView");    
}

void Renderer::initNormalsCopy(int width, int height)
{
    Globals * globals = Globals::getInstance();
    HRESULT result;
    
    D3D11_TEXTURE2D_DESC texture_desc;
    ZeroMemory(&texture_desc, sizeof(texture_desc));
    texture_desc.Width = width;
    texture_desc.Height = height;
    texture_desc.MipLevels = 1;
    texture_desc.ArraySize = 1;
    texture_desc.Format = DXGI_FORMAT_R16G16B16A16_SNORM;
    texture_desc.SampleDesc.Count = MSAA_SAMPLES_COUNT;
    texture_desc.Usage = D3D11_USAGE_DEFAULT;
    texture_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    texture_desc.CPUAccessFlags = 0;
    texture_desc.MiscFlags = 0;

    result = globals->device5->CreateTexture2D(&texture_desc,
                                               NULL,
                                               normals_copy.reset());
    assert(result >= 0 && "CreateTexture2D");
    
    D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
    ZeroMemory(&srv_desc, sizeof(srv_desc));
    srv_desc.Format = texture_desc.Format;
    srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srv_desc.Texture2D.MostDetailedMip = 0;
    srv_desc.Texture2D.MipLevels = 1;

    result = globals->device5->CreateShaderResourceView(normals_copy.ptr(),
                                                        &srv_desc,
                                                        normals_copy_srv.reset());
    assert(result >= 0 && "CreateShaderResourceView");
}

void Renderer::initAlbedoTexture(int width, int height)
{
    Globals * globals = Globals::getInstance();
    HRESULT result;
    
    D3D11_TEXTURE2D_DESC texture_desc;
    ZeroMemory(&texture_desc, sizeof(texture_desc));
    texture_desc.Width = width;
    texture_desc.Height = height;
    texture_desc.MipLevels = 1;
    texture_desc.ArraySize = 1;
    texture_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texture_desc.SampleDesc.Count = MSAA_SAMPLES_COUNT;
    texture_desc.Usage = D3D11_USAGE_DEFAULT;
    texture_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    texture_desc.CPUAccessFlags = 0;
    texture_desc.MiscFlags = 0;

    result = globals->device5->CreateTexture2D(&texture_desc,
                                               NULL,
                                               albedo.reset());
    assert(result >= 0 && "CreateTexture2D");
    
    D3D11_RENDER_TARGET_VIEW_DESC rtv_desc;
    ZeroMemory(&rtv_desc, sizeof(rtv_desc));
    rtv_desc.Format = texture_desc.Format;
    rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    rtv_desc.Texture2D.MipSlice = 0;

    result = globals->device5->CreateRenderTargetView(albedo.ptr(),
                                                      &rtv_desc,
                                                      albedo_rtv.reset());
    assert(result >= 0 && "CreateRenderTargetView");

    D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
    ZeroMemory(&srv_desc, sizeof(srv_desc));
    srv_desc.Format = texture_desc.Format;
    srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srv_desc.Texture2D.MostDetailedMip = 0;
    srv_desc.Texture2D.MipLevels = 1;

    result = globals->device5->CreateShaderResourceView(albedo.ptr(),
                                                        &srv_desc,
                                                        albedo_srv.reset());
    assert(result >= 0 && "CreateShaderResourceView");    
}

void Renderer::initRoughnessMetalnessTexture(int width, int height)
{
    Globals * globals = Globals::getInstance();
    HRESULT result;
    
    D3D11_TEXTURE2D_DESC texture_desc;
    ZeroMemory(&texture_desc, sizeof(texture_desc));
    texture_desc.Width = width;
    texture_desc.Height = height;
    texture_desc.MipLevels = 1;
    texture_desc.ArraySize = 1;
    texture_desc.Format = DXGI_FORMAT_R8G8_UNORM;
    texture_desc.SampleDesc.Count = MSAA_SAMPLES_COUNT;
    texture_desc.Usage = D3D11_USAGE_DEFAULT;
    texture_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    texture_desc.CPUAccessFlags = 0;
    texture_desc.MiscFlags = 0;

    result = globals->device5->CreateTexture2D(&texture_desc,
                                               NULL,
                                               roughness_metalness.reset());
    assert(result >= 0 && "CreateTexture2D");
    
    D3D11_RENDER_TARGET_VIEW_DESC rtv_desc;
    ZeroMemory(&rtv_desc, sizeof(rtv_desc));
    rtv_desc.Format = texture_desc.Format;
    rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    rtv_desc.Texture2D.MipSlice = 0;

    result = globals->device5->CreateRenderTargetView(roughness_metalness.ptr(),
                                                      &rtv_desc,
                                                      roughness_metalness_rtv.reset());
    assert(result >= 0 && "CreateRenderTargetView");

    D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
    ZeroMemory(&srv_desc, sizeof(srv_desc));
    srv_desc.Format = texture_desc.Format;
    srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srv_desc.Texture2D.MostDetailedMip = 0;
    srv_desc.Texture2D.MipLevels = 1;

    result = globals->device5->CreateShaderResourceView(roughness_metalness.ptr(),
                                                        &srv_desc,
                                                        roughness_metalness_srv.reset());
    assert(result >= 0 && "CreateShaderResourceView");
}

// .rgb - emissive
// .a - ambient occlusion
void Renderer::initEmissiveTexture(int width, int height)
{
    Globals * globals = Globals::getInstance();
    HRESULT result;
    
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
                                               emissive_ao.reset());
    assert(result >= 0 && "CreateTexture2D");
    
    D3D11_RENDER_TARGET_VIEW_DESC rtv_desc;
    ZeroMemory(&rtv_desc, sizeof(rtv_desc));
    rtv_desc.Format = texture_desc.Format;
    rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    rtv_desc.Texture2D.MipSlice = 0;

    result = globals->device5->CreateRenderTargetView(emissive_ao.ptr(),
                                                      &rtv_desc,
                                                      emissive_ao_rtv.reset());
    assert(result >= 0 && "CreateRenderTargetView");

    D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
    ZeroMemory(&srv_desc, sizeof(srv_desc));
    srv_desc.Format = texture_desc.Format;
    srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srv_desc.Texture2D.MostDetailedMip = 0;
    srv_desc.Texture2D.MipLevels = 1;

    result = globals->device5->CreateShaderResourceView(emissive_ao.ptr(),
                                                        &srv_desc,
                                                        emissive_ao_srv.reset());
    assert(result >= 0 && "CreateShaderResourceView");    
}

void Renderer::initModelIDTexture(int width, int height)
{
    Globals * globals = Globals::getInstance();
    HRESULT result;
    
    D3D11_TEXTURE2D_DESC texture_desc;
    ZeroMemory(&texture_desc, sizeof(texture_desc));
    texture_desc.Width = width;
    texture_desc.Height = height;
    texture_desc.MipLevels = 1;
    texture_desc.ArraySize = 1;
    texture_desc.Format = DXGI_FORMAT_R16_UINT;
    texture_desc.SampleDesc.Count = MSAA_SAMPLES_COUNT;
    texture_desc.Usage = D3D11_USAGE_DEFAULT;
    texture_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    texture_desc.CPUAccessFlags = 0;
    texture_desc.MiscFlags = 0;

    result = globals->device5->CreateTexture2D(&texture_desc,
                                               NULL,
                                               model_id.reset());
    assert(result >= 0 && "CreateTexture2D");
    
    D3D11_RENDER_TARGET_VIEW_DESC rtv_desc;
    ZeroMemory(&rtv_desc, sizeof(rtv_desc));
    rtv_desc.Format = texture_desc.Format;
    rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    rtv_desc.Texture2D.MipSlice = 0;

    result = globals->device5->CreateRenderTargetView(model_id.ptr(),
                                                      &rtv_desc,
                                                      model_id_rtv.reset());
    assert(result >= 0 && "CreateRenderTargetView");

    D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
    ZeroMemory(&srv_desc, sizeof(srv_desc));
    srv_desc.Format = texture_desc.Format;
    srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srv_desc.Texture2D.MostDetailedMip = 0;
    srv_desc.Texture2D.MipLevels = 1;

    result = globals->device5->CreateShaderResourceView(model_id.ptr(),
                                                        &srv_desc,
                                                        model_id_srv.reset());
    assert(result >= 0 && "CreateShaderResourceView");    
}

void Renderer::copyNormals()
{
    Globals * globals = Globals::getInstance();

    globals->device_context4->CopyResource(normals_copy.ptr(),
                                           normals.ptr());
}

void Renderer::deferredShading()
{
    Globals * globals = Globals::getInstance();
    LightSystem * light_sys = LightSystem::getInstance();

    globals->bindDefaultBlendState();
    
    globals->device_context4->
        IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    bindRenderTarget(true);
    clearRenderTarget();

    changeDepthBufferAccess(true);

    bindGBufferSRV();

    copyDepthBuffer();
    globals->device_context4->PSSetShaderResources(5,
                                                   1,
                                                   depth_copy_srv.get());
    
    deferred_shader->bind();
    reflectance->bind(6);
    irradiance->bind(7);
    reflection->bind(8);

    light_sys->bindShadowMapSRV(9);
    
    globals->device_context4->Draw(3, 0);

    changeDepthBufferAccess(false);
}
} // namespace engine

