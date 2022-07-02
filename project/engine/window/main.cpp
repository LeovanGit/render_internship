#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <d3dcompiler.h>

#include <iostream>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "D3DCompiler.lib")

namespace
{
    // deep blue
    constexpr float BACKGROUND[4] = {0.0f, 0.2f, 0.4f, 1.0f};
    
    constexpr int SCREEN_WIDTH = 800;
    constexpr int SCREEN_HEIGHT = 600;
}

struct Vertex
{
    float x, y, z;
    float color[4];
};

ID3D11Device * device;
ID3D11DeviceContext * device_context;
IDXGISwapChain * swapchain;
ID3D11RenderTargetView * backbuffer;

ID3D11VertexShader * m_vertexShader;
ID3D11PixelShader * m_pixelShader;

ID3D11Buffer * VBO;
ID3D11InputLayout * pLayout;

bool InitShader(WCHAR * vert_filename, WCHAR * frag_filename)
{
    HRESULT result;
    ID3D10Blob * errorMessage(0);
    ID3D10Blob * vertexShaderBuffer(0);
    ID3D10Blob * pixelShaderBuffer(0);
    D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
    unsigned int numElements;
    D3D11_BUFFER_DESC matrixBufferDesc;

    // Compile the vertex shader code
    result = D3DCompileFromFile(vert_filename,
                                NULL,
                                NULL,
                                "vertexShader",
                                "vs_5_0",
                                D3D10_SHADER_ENABLE_STRICTNESS,
                                0,
                                &vertexShaderBuffer,
                                &errorMessage);

    if(FAILED(result))
    {
        if (errorMessage) std::cout << "\nVERTEX SHADER COMPILATION ERROR:\n"
                                    << (char *)errorMessage->GetBufferPointer()
                                    << "\n\n";
        else std::cout << "\nVERTEX SHADER NOT FOUND\n";

        return false;
    }

    // Compile the fragment shader code
    result = D3DCompileFromFile(frag_filename,
                                NULL,
                                NULL,
                                "fragmentShader",
                                "ps_5_0",
                                D3D10_SHADER_ENABLE_STRICTNESS,
                                0,
                                &pixelShaderBuffer,
                                &errorMessage);

    if(FAILED(result))
    {
        if (errorMessage) std::cout << "\nFRAGMENT SHADER COMPILATION ERROR:\n" 
                                    << (char *)errorMessage->GetBufferPointer()
                                    << "\n\n";
        else std::cout << "FRAGMENT SHADER NOT FOUND\n";

        return false;
    }

    // Create the vertex shader from the buffer
    result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(),
                                        vertexShaderBuffer->GetBufferSize(),
                                        NULL,
                                        &m_vertexShader);

    // Create the fragment shader from the buffer
    result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(),
                                       pixelShaderBuffer->GetBufferSize(),
                                       NULL,
                                       &m_pixelShader);

    // activate shaders
    device_context->VSSetShader(m_vertexShader, 0, 0);
    device_context->PSSetShader(m_pixelShader, 0, 0);

    // CREATE INPUT LAYOUT (VAO)
    D3D11_INPUT_ELEMENT_DESC ied[] =
    {
    // use D3D11_APPEND_ALIGNED_ELEMENT to auto calculate offset
        {"POSITION",
         0,
         DXGI_FORMAT_R32G32B32_FLOAT,
         0,
         0,
         D3D11_INPUT_PER_VERTEX_DATA,
         0},

        {"COLOR",
         0,
         DXGI_FORMAT_R32G32B32A32_FLOAT,
         0,
         12, // offset
         D3D11_INPUT_PER_VERTEX_DATA,
         0},
    };

    device->CreateInputLayout(ied,
                              2,
                              vertexShaderBuffer->GetBufferPointer(),
                              vertexShaderBuffer->GetBufferSize(),
                              &pLayout);

    device_context->IASetInputLayout(pLayout);

    return true;
}

void InitGraphics()
{
    Vertex vertices[] =
    {
        Vertex{ 0.0f,   0.5f, 0.0f, {1.0f, 0.0f, 0.0f, 1.0f}},
        Vertex{ 0.45f, -0.5f, 0.0f, {0.0f, 1.0f, 0.0f, 1.0f}},
        Vertex{-0.45f, -0.5f, 0.0f, {0.0f, 0.0f, 1.0f, 1.0f}}
    };

    // CREATE VERTEX BUFFER
    // contain properties of the buffer
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));

    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(Vertex) * 3;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    device->CreateBuffer(&bd, NULL, &VBO);

    // copy vertices into the buffer
    // ms contain info about the buffer, 
    // ms.pData - pointer to the buffer's location
    D3D11_MAPPED_SUBRESOURCE ms;
    device_context->Map(VBO, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
    memcpy(ms.pData, vertices, sizeof(vertices));
    device_context->Unmap(VBO, NULL);
}

// init COM objects
void InitD3D(HWND hwnd)
{
    // holds information about the swap chain
    DXGI_SWAP_CHAIN_DESC scd;

    // init the entire scd with zeros
    ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

    // fill the swap chain description struct
    scd.BufferCount = 1;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferDesc.Width = SCREEN_WIDTH;
    scd.BufferDesc.Height = SCREEN_HEIGHT;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; 
    scd.OutputWindow = hwnd;
    scd.SampleDesc.Count = 4;
    scd.Windowed = TRUE;
    scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    // create a device, device context and swap chain
    // using the information in the scd struct
    D3D11CreateDeviceAndSwapChain(NULL,
                                  D3D_DRIVER_TYPE_HARDWARE,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  D3D11_SDK_VERSION,
                                  &scd,
                                  &swapchain,
                                  &device,
                                  NULL,
                                  &device_context);

    // SET THE RENDER TARGET
    // get the address of the back buffer
    ID3D11Texture2D * pBackBuffer;
    swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

    // use the back buffer address to create the render target
    device->CreateRenderTargetView(pBackBuffer, NULL, &backbuffer);
    pBackBuffer->Release();

    // set the render target as the back buffer
    device_context->OMSetRenderTargets(1, &backbuffer, NULL);

    // SET THE VIEWPORT
    D3D11_VIEWPORT viewport;
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = SCREEN_WIDTH;
    viewport.Height = SCREEN_HEIGHT;

    device_context->RSSetViewports(1, &viewport);

    InitShader(L"../engine/shaders/shader.vert",
               L"../engine/shaders/shader.frag");

    InitGraphics();
}

// release all existing COM objects
void CleanD3D()
{
    // switch back to windowed mode before end
    swapchain->SetFullscreenState(FALSE, NULL);

    swapchain->Release();
    device->Release();
    device_context->Release();
    backbuffer->Release();

    // if they don't created we will refer to nullptr
    if (m_vertexShader) m_vertexShader->Release();
    if (m_pixelShader) m_pixelShader->Release();
}

void RenderFrame(void)
{
    // fill the back buffer to a background color
    device_context->ClearRenderTargetView(backbuffer,
                                          BACKGROUND);

    // RENDER TO BACK BUFFER
    // which VBO to use
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    device_context->IASetVertexBuffers(0, 1, &VBO, &stride, &offset);

    // which type of primitive to use (triangles)
    device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // draw VBO to the back buffer
    device_context->Draw(3, 0);

    // switch the back buffer and the front buffer
    swapchain->Present(0, 0);
}

LRESULT CALLBACK WindowProc(HWND hWnd,
                            UINT message,
                            WPARAM wParam,
                            LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    HWND hWnd;
    WNDCLASSEX wc;

    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    // wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszClassName = "WindowClass";

    RegisterClassEx(&wc);

    // calculate the window size consided on client area
    RECT wr = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

    // CREATE THE WINDOW
    hWnd = CreateWindowEx(NULL,
                          "WindowClass",
                          "init Direct3D",
                          WS_OVERLAPPEDWINDOW,
                          300,
                          300,
                          wr.right - wr.left,
                          wr.bottom - wr.top,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hWnd, nCmdShow);

    // set up and initialize Direct3D
    InitD3D(hWnd);

    // MAIN LOOP
    MSG msg;

    while(true)
    {
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if(msg.message == WM_QUIT) break;
        }
        else
        {
            // game code
            RenderFrame();
        }
    }

    // clean up DirectX and COM
    CleanD3D();

    return msg.wParam;
}

// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
        case WM_DESTROY:
            {
                PostQuitMessage(0);
                return 0;
            } break;
    }

    return DefWindowProc (hWnd, message, wParam, lParam);
}

