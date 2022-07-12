#include "window.hpp"
#include "ext/matrix_clip_space.hpp"
#include "texture_manager.hpp"

namespace
{
constexpr float QUALITY = 2.0f;
constexpr float BACKGROUND[4] = {0.4f, 0.44f, 0.4f, 1.0f};
} // namespace

namespace engine::windows
{
void Window::init(HINSTANCE hInstance,
                  int pos_x, int pos_y,
                  int client_width, int client_height)
{
    this->pos_x = pos_x;
    this->pos_y = pos_y;
    this->client_width = client_width;
    this->client_height = client_height;

    // calculate the window size consided on client area
    RECT window = {0, 0, client_width, client_height};
    AdjustWindowRect(&window, WS_OVERLAPPEDWINDOW, FALSE);

    handle = CreateWindowEx(0,
                            "window_class_1",
                            "raytracing",
                            WS_OVERLAPPEDWINDOW,
                            pos_x,
                            pos_y,
                            window.right - window.left,
                            window.bottom - window.top,
                            NULL,
                            NULL,
                            hInstance,
                            NULL);

    hdc = GetDC(handle);

    initSwapChain();
    initBackBuffer();
    initViewport();
    initDepthBuffer();
}

void Window::initSwapChain()
{
    // holds information about the swap chain
    DXGI_SWAP_CHAIN_DESC1 scd;
    memset(&scd, 0, sizeof(DXGI_SWAP_CHAIN_DESC1));
    scd.AlphaMode = DXGI_ALPHA_MODE::DXGI_ALPHA_MODE_UNSPECIFIED;
    scd.BufferCount = 2;
    scd.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.Flags = 0;
    scd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.SampleDesc.Count = 1;
    scd.SampleDesc.Quality = 0;
    scd.Scaling = DXGI_SCALING_NONE;
    scd.Stereo = false;
    scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    scd.Width = 0; // auto sizing
    scd.Height = 0;

    // get access to global render variables
    Globals * globals = Globals::getInstance();

    HRESULT result = globals->factory5->
        CreateSwapChainForHwnd(globals->device5,
                               handle,
                               &scd,
                               NULL,
                               NULL,
                               m_swapchain.reset());

    assert(result >= 0 && "CreateSwapChainForHwnd");
}

void Window::initBackBuffer()
{
    // may be called after resizing
    if (m_backbuffer.valid())
    {
        m_backbuffer.release();
        m_render_target.release();

        // before ResizeBuffers(), backbuffers must be released!!!
        HRESULT result = m_swapchain->ResizeBuffers(0,
                                                    0, // auto sizing
                                                    0,
                                                    DXGI_FORMAT_UNKNOWN,
                                                    0);
        assert(result >= 0 && "ResizeBuffers");
    }
    
    HRESULT result = m_swapchain->GetBuffer(0,
                                            __uuidof(ID3D11Texture2D),
                                            (void **)m_backbuffer.reset());
    assert(result >= 0);

    // get backbuffer description
    ID3D11Texture2D * pTextureInterface = 0;
    m_backbuffer->QueryInterface<ID3D11Texture2D>(&pTextureInterface);
    pTextureInterface->GetDesc(&m_backbuffer_desc);
    pTextureInterface->Release();

    // get access to global render variables
    Globals * globals = Globals::getInstance();

    // CREATE RENDER TARGET
    result = globals->device5->
        CreateRenderTargetView(m_backbuffer.ptr(),
                               NULL,
                               m_render_target.reset());

    assert(result >= 0 && "CreateRenderTargetView");
}

void Window::initDepthBuffer()
{
    HRESULT result;
    Globals * globals = Globals::getInstance();

    // create depth buffer
    D3D11_TEXTURE2D_DESC dsb_desc;
    ZeroMemory(&dsb_desc, sizeof(dsb_desc));
    dsb_desc.Width = client_width;
    dsb_desc.Height = client_height;
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
}

void Window::initViewport()
{
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = client_width;
    viewport.Height = client_height;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
}

void Window::resize(int width, int height)
{
    client_width = width;
    client_height = height;

    initBackBuffer();
    initViewport();
}

RECT Window::getSize() const
{
    RECT window;
    GetWindowRect(handle, &window);
    return window;
}

RECT Window::getClientSize() const
{
    RECT client;
    GetClientRect(handle, &client);
    return client;
}

void Window::renderFrame()
{
    HRESULT result;
    Globals * globals = Globals::getInstance();
    TextureManager * tex_mgr = TextureManager::getInstance();
    ShaderManager * shader_mgr = ShaderManager::getInstance();

    // write new data to const buffer
    D3D11_MAPPED_SUBRESOURCE ms;
    result = globals->device_context4->Map(globals->const_buffer.ptr(),
                                           NULL,
                                           D3D11_MAP_WRITE_DISCARD,
                                           NULL,
                                           &ms);
    assert(result >= 0 && "Map");

    memcpy(ms.pData,
           &globals->const_buffer_data,
           sizeof(globals->const_buffer_data));

    globals->device_context4->Unmap(globals->const_buffer.ptr(), NULL);

    // bind const buffer with updated values to vertex shader
    globals->device_context4->VSSetConstantBuffers(0,
                                                   1,
                                                   globals->const_buffer.get());

    // set render target and depth buffer
    globals->device_context4->OMSetRenderTargets(1,
                                                 m_render_target.get(),
                                                 depth_stencil_view.ptr());

    // set viewport
    globals->device_context4->RSSetViewports(1, &viewport);

    // fill the back buffer with background color
    globals->device_context4->ClearRenderTargetView(m_render_target.ptr(),
                                                    BACKGROUND);

    // clear depth buffer
    globals->device_context4->ClearDepthStencilView(depth_stencil_view.ptr(),
                                                    D3D11_CLEAR_DEPTH |
                                                    D3D11_CLEAR_STENCIL,
                                                    1.0f,
                                                    0);

    // bind sampler to fragment shader
    globals->device_context4->PSSetSamplers(0,
                                            1,
                                            tex_mgr->getTexture(0).sampler_state.get());
    
    // bind texture to fragment shader
    globals->device_context4->
        PSSetShaderResources(0,
                             1,
                             tex_mgr->getTexture(0).texture_view.get());

    // bind shader and input layout
    shader_mgr->useShader(0);

    // which VBO to use
    uint32_t stride = sizeof(Vertex);
    uint32_t offset = 0;
    globals->device_context4->IASetVertexBuffers(0,
                                                 1,
                                                 globals->vbo.get(),
                                                 &stride,
                                                 &offset);

    // which type of primitive to use (triangles)
    globals->device_context4->
        IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // draw VBO to the back buffer
    globals->device_context4->Draw(36, 0);

    // DRAW FULLSCREEN TRIANGLE (WITHOUT VBO -> GENERATED IN SHADER)
    shader_mgr->useShader(1);

    // bind skybox sampler to fragment shader
    globals->device_context4->PSSetSamplers(0,
                                            1,
                                            tex_mgr->getTexture(1).sampler_state.get());

    // bind skybox cubemap to fragment shader
    globals->device_context4->
        PSSetShaderResources(0,
                             1,
                             tex_mgr->getTexture(1).texture_view.get());

    globals->device_context4->Draw(3, 0);

    // DRAW FLOOR
    shader_mgr->useShader(2);

    // bind sampler to fragment shader
    globals->device_context4->PSSetSamplers(0,
                                            1,
                                            tex_mgr->getTexture(2).sampler_state.get());
    
    // bind texture to fragment shader
    globals->device_context4->
        PSSetShaderResources(0,
                             1,
                             tex_mgr->getTexture(2).texture_view.get());

    globals->device_context4->Draw(6, 0);

    // switch the back buffer and the front buffer
    m_swapchain->Present(0, 0);
}
} // namespace engine::windows
