#include "globals.hpp"

// Say NVidia or AMD driver to prefer a dedicated GPU instead of an integrated
// This has effect on laptops
extern "C"
{
    _declspec(dllexport) uint32_t NvOptimusEnablement = 1;
    _declspec(dllexport) uint32_t AmdPowerXpressRequestHighPerformance = 1;
}

namespace engine
{
void Globals::initD3D()
{
    HRESULT result;

    // FACTORY
    result = CreateDXGIFactory(__uuidof(IDXGIFactory),
                               (void **)m_factory.reset());
    assert(result >= 0 && "CreateDXGIFactory");

    result = m_factory->QueryInterface(__uuidof(IDXGIFactory5),
                                       (void**)m_factory5.reset());
    assert(result >= 0 && "Query IDXGIFactory5");

    // ADAPTERS (provide info about your GPUs)
    {
        uint32_t index = 0;
        IDXGIAdapter1 * adapter;

        while (m_factory5->EnumAdapters1(index++,
                                         &adapter) != DXGI_ERROR_NOT_FOUND)
        {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);
            
            std::cout << "\nGPU #" << index << " " << desc.Description << "\n";
        }
    }

    // DEVICE AND DEVICE CONTEXT
    const D3D_FEATURE_LEVEL featureLevelRequested = D3D_FEATURE_LEVEL_11_0;
    D3D_FEATURE_LEVEL featureLevelInitialized = D3D_FEATURE_LEVEL_11_0;

    result = D3D11CreateDevice(nullptr,
                               D3D_DRIVER_TYPE_HARDWARE,
                               nullptr,
                               D3D11_CREATE_DEVICE_DEBUG,
                               &featureLevelRequested,
                               1,
                               D3D11_SDK_VERSION,
                               m_device.reset(),
                               &featureLevelInitialized,
                               m_device_context.reset());

    assert(result >= 0 && "D3D11CreateDevice");
    assert(featureLevelRequested == featureLevelInitialized &&
           "D3D_FEATURE_LEVEL_11_0");

    result = m_device->QueryInterface(__uuidof(ID3D11Device5),
                                      (void**)m_device5.reset());
    assert(result >= 0 && "Query ID3D11Device5");

    result = m_device_context->QueryInterface(__uuidof(ID3D11DeviceContext4), 
                                              (void**)m_device_context4.reset());
    assert(result >= 0 && "Query ID3D11DeviceContext4");

    result = m_device->QueryInterface(__uuidof(ID3D11Debug),
                                      (void**)m_device_debug.reset());
    assert(result >= 0 && "Query ID3D11Debug");

    // Write global pointers​
    s_factory = m_factory5.ptr();
    s_device = m_device5.ptr();
    s_device_context = m_device_context4.ptr();
}

void Globals::initVBO()
{
    Vertex vertices[] =
    {
        //     POSITION                COLOR
        Vertex{ 0.0f,    0.5f,  0.0f, {1.0f, 0.0f, 0.0f, 1.0f}},
        Vertex{ 0.433f, -0.25f, 0.0f, {0.0f, 1.0f, 0.0f, 1.0f}},
        Vertex{-0.433f, -0.25f, 0.0f, {0.0f, 0.0f, 1.0f, 1.0f}}
    };

    // contain properties of the VBO
    D3D11_BUFFER_DESC vbo_desc;
    ZeroMemory(&vbo_desc, sizeof(vbo_desc));

    vbo_desc.Usage = D3D11_USAGE_DYNAMIC;
    vbo_desc.ByteWidth = sizeof(Vertex) * 3;
    vbo_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbo_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    HRESULT result = s_device->CreateBuffer(&vbo_desc,
                                            NULL,
                                            &s_vbo);
    assert(result >= 0 && "CreateBuffer");

    // copy vertices into the buffer
    // ms contain info about the buffer, 
    // ms.pData - pointer to the buffer's location
    D3D11_MAPPED_SUBRESOURCE ms;
    s_device_context->Map(s_vbo, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
    memcpy(ms.pData, vertices, sizeof(vertices));
    s_device_context->Unmap(s_vbo, NULL);
}

void Globals::cleanD3D()
{
    if (s_device) s_device->Release();
    if (s_device_context) s_device_context->Release();
    if (s_factory) s_factory->Release();

    if (s_vbo) s_vbo->Release();
    if (s_input_layout) s_input_layout->Release();
}

} // namespace engine