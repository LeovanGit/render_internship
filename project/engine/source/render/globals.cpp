#include "globals.hpp"

// Say Nvidia/AMD driver to prefer a dedicated GPU instead of an integrated
extern "C"
{
    _declspec(dllexport) uint32_t NvOptimusEnablement = 1;
    _declspec(dllexport) uint32_t AmdPowerXpressRequestHighPerformance = 1;
}

namespace engine
{
Globals * Globals::instance = nullptr;

void Globals::init()
{
    if (!instance)
    {
        instance = new Globals();

        instance->initD3D();
        instance->initSamplers();
    }
    else spdlog::error("Globals::init() was called twice!");
}

Globals * Globals::getInstance()
{
    return instance;
}

void Globals::del()
{
    if (instance)
    {
        delete instance;
        instance = nullptr;
    }
    else spdlog::error("Globals::del() was called twice!");
}

void Globals::bind(const Camera & camera)
{
    setPerFrameBuffer(camera);
    updatePerFrameBuffer();

    // bind sampler to fragment shader
    device_context4->PSSetSamplers(0,
                                   1,
                                   sampler.get());
}

void Globals::initD3D()
{
    HRESULT result;

    // FACTORY
    result = CreateDXGIFactory(__uuidof(IDXGIFactory),
                              (void **)factory.reset());
    assert(result >= 0 && "CreateDXGIFactory");

    result = factory->QueryInterface(__uuidof(IDXGIFactory5),
                                    (void**)factory5.reset());
    assert(result >= 0 && "Query IDXGIFactory5");

    // ADAPTERS (provide info about your GPUs)
    {
        uint32_t index = 0;
        IDXGIAdapter1 * adapter;

        std::cout << "\n===> AVAILABLE ADAPTERS <===\n";

        while (factory5->EnumAdapters1(index++,
                                       &adapter) != DXGI_ERROR_NOT_FOUND)
        {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);
            
            std::wcout << "GPU #" << index << " " << desc.Description << "\n";
        }

        std::cout << "============================\n";
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
                               device.reset(),
                               &featureLevelInitialized,
                               device_context.reset());

    assert(result >= 0 && "D3D11CreateDevice");
    assert(featureLevelRequested == featureLevelInitialized &&
           "D3D_FEATURE_LEVEL_11_0");

    result = device->QueryInterface(__uuidof(ID3D11Device5),
                                   (void**)device5.reset());
    assert(result >= 0 && "Query ID3D11Device5");

    result = device_context->QueryInterface(__uuidof(ID3D11DeviceContext4), 
                                            (void**)device_context4.reset());
    assert(result >= 0 && "Query ID3D11DeviceContext4");

    result = device->QueryInterface(__uuidof(ID3D11Debug),
                                   (void**)device_debug.reset());
    assert(result >= 0 && "Query ID3D11Debug");
}

void Globals::initSamplers()
{
    D3D11_SAMPLER_DESC sampler_desc;
    ZeroMemory(&sampler_desc, sizeof(sampler_desc));
    sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    // sampler_desc.Filter = D3D11_FILTER_ANISOTROPIC;
    // sampler_desc.MaxAnisotropy = 16;
    sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc.MinLOD = 0;
    sampler_desc.MaxLOD = D3D11_FLOAT32_MAX; // unlimited mipmap levels

    HRESULT result = device5->CreateSamplerState(&sampler_desc,
                                                 sampler.reset());
    assert(result >= 0 && "CreateSamplerState");
}

void Globals::setPerFrameBuffer(const Camera & camera)
{
    // find CS corner points in WS
    glm::vec3 bottom_left_WS = camera.reproject(-1.0f, -1.0f);
    glm::vec3 top_left_WS = camera.reproject(-1.0f, 1.0f);
    glm::vec3 bottom_right_WS = camera.reproject(1.0f, -1.0f);

    // fill const buffer data
    per_frame_buffer_data.g_proj_view = camera.getViewProj();
    per_frame_buffer_data.g_camera_pos = camera.getPosition();
    per_frame_buffer_data.g_frustum_corners[0] = glm::vec4(bottom_left_WS, 1.0f);
    per_frame_buffer_data.g_frustum_corners[1] = glm::vec4(top_left_WS, 1.0f);
    per_frame_buffer_data.g_frustum_corners[2] = glm::vec4(bottom_right_WS, 1.0f);

    if (per_frame_buffer.valid()) return;
    // constant buffer description
    D3D11_BUFFER_DESC cb_desc;
    cb_desc.Usage = D3D11_USAGE_DYNAMIC;
    cb_desc.ByteWidth = sizeof(per_frame_buffer_data);
    cb_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cb_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cb_desc.MiscFlags = 0;
    cb_desc.StructureByteStride = 0;
    
    HRESULT result = device5->CreateBuffer(&cb_desc,
                                           NULL,
                                           per_frame_buffer.reset());
    assert(result >= 0 && "CreateBuffer");
}

void Globals::updatePerFrameBuffer()
{
    HRESULT result;

    // write new data to const buffer
    D3D11_MAPPED_SUBRESOURCE ms;
    result = device_context4->Map(per_frame_buffer.ptr(),
                                  NULL,
                                  D3D11_MAP_WRITE_DISCARD,
                                  NULL,
                                  &ms);
    assert(result >= 0 && "Map");

    memcpy(ms.pData,
           &per_frame_buffer_data,
           sizeof(per_frame_buffer_data));

    device_context4->Unmap(per_frame_buffer.ptr(), NULL);

    // bind const buffer with updated values to vertex shader
    device_context4->VSSetConstantBuffers(0,
                                          1,
                                          per_frame_buffer.get());
}

void Globals::setPerMeshBuffer(const glm::mat4 & mesh_to_model)
{
    // fill const buffer data
    per_mesh_buffer_data.mesh_to_model = mesh_to_model;

    if (per_mesh_buffer.valid()) return;
    // constant buffer description
    D3D11_BUFFER_DESC cb_desc;
    cb_desc.Usage = D3D11_USAGE_DYNAMIC;
    cb_desc.ByteWidth = sizeof(per_mesh_buffer_data);
    cb_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cb_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cb_desc.MiscFlags = 0;
    cb_desc.StructureByteStride = 0;
    
    HRESULT result = device5->CreateBuffer(&cb_desc,
                                           NULL,
                                           per_mesh_buffer.reset());
    assert(result >= 0 && "CreateBuffer");
}

void Globals::updatePerMeshBuffer()
{
    HRESULT result;

    // write new data to const buffer
    D3D11_MAPPED_SUBRESOURCE ms;
    result = device_context4->Map(per_mesh_buffer.ptr(),
                                  NULL,
                                  D3D11_MAP_WRITE_DISCARD,
                                  NULL,
                                  &ms);
    assert(result >= 0 && "Map");

    memcpy(ms.pData,
           &per_mesh_buffer_data,
           sizeof(per_mesh_buffer_data));

    device_context4->Unmap(per_mesh_buffer.ptr(), NULL);

    // bind const buffer with updated values to vertex shader
    device_context4->VSSetConstantBuffers(1,
                                          1,
                                          per_mesh_buffer.get());
}
} // namespace engine
