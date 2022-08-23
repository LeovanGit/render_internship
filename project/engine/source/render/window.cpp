#include "window.hpp"

namespace
{
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
                               swapchain.reset());

    assert(result >= 0 && "CreateSwapChainForHwnd");
}

void Window::initBackBuffer()
{
    Globals * globals = Globals::getInstance();
    HRESULT result;
    
    // may be called after resizing
    if (backbuffer.valid())
    {
        backbuffer.release();
        LDR_RTV.release();

        // before ResizeBuffers(), all backbuffers must be released!!!
        result = swapchain->ResizeBuffers(0,
                                          0, // auto size
                                          0,
                                          DXGI_FORMAT_UNKNOWN,
                                          0);
        assert(result >= 0 && "ResizeBuffers");
    }
    
    result = swapchain->GetBuffer(0,
                                  __uuidof(ID3D11Texture2D),
                                  (void **)backbuffer.reset());
    assert(result >= 0 && "GetBuffer");

    // get backbuffer description
    ID3D11Texture2D * pTextureInterface = 0;
    backbuffer->QueryInterface<ID3D11Texture2D>(&pTextureInterface);
    pTextureInterface->GetDesc(&backbuffer_desc);
    pTextureInterface->Release();

    // create LDR render target
    result = globals->device5->CreateRenderTargetView(backbuffer.ptr(),
                                                      NULL,
                                                      LDR_RTV.reset());
    assert(result >= 0 && "CreateRenderTargetView");
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

void Window::clearFrame()
{
    Globals * globals = Globals::getInstance();

    // fill the back buffer with background color
    globals->device_context4->ClearRenderTargetView(LDR_RTV.ptr(),
                                                    BACKGROUND);
}

void Window::bindRenderTarget()
{
    Globals * globals = Globals::getInstance();

    globals->device_context4->OMSetRenderTargets(1,
                                                 LDR_RTV.get(),
                                                 NULL);
}

void Window::bindViewport()
{
    Globals * globals = Globals::getInstance();

    globals->device_context4->RSSetViewports(1, &viewport);
}

void Window::switchBuffer()
{
    // switch the back buffer and the front buffer
    swapchain->Present(0, 0);
}
} // namespace engine::windows
