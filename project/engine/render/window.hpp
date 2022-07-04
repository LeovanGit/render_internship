#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "win_def.hpp"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <windowsx.h>
#include <d3d11_4.h>
#include <d3dcompiler.h>
#include <cassert>
#include <vector>

#include "dx_res_ptr.hpp"

#include "win_undef.hpp"

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "D3DCompiler.lib")

namespace engine::windows
{
class Window
{
public:
    Window() = default;

    void init(HINSTANCE hInstance,
              int pos_x, int pos_y,
              int client_width, int client_height);

    void initSwapChain();

    void initBackBuffer();

    void initViewport();

    void resize(int width, int height);

    RECT getSize() const;

    RECT getClientSize() const;

    void renderFrame();

    HWND handle;
    HDC hdc;

    int pos_x;
    int pos_y;

private:    
    int client_width;
    int client_height;

    DxResPtr<IDXGISwapChain1> m_swapchain;
    DxResPtr<ID3D11Texture2D> m_backbuffer;
    D3D11_TEXTURE2D_DESC m_backbuffer_desc;
    DxResPtr<ID3D11RenderTargetView> m_render_target;
    D3D11_VIEWPORT viewport;
};
} // namespace engine::windows

#endif
