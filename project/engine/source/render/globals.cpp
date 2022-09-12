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
        instance->initRasterizers();
        instance->initPerFrameBuffer();
        instance->initPerMeshBuffer();
        instance->initPerEmissiveMeshBuffer();
        instance->initPerShadowMeshBuffer();
        instance->initPerShadowCubeMapBuffer();
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
    HRESULT result;
    
    // default
    D3D11_SAMPLER_DESC sampler_desc;
    ZeroMemory(&sampler_desc, sizeof(sampler_desc));
    // sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampler_desc.Filter = D3D11_FILTER_ANISOTROPIC;
    sampler_desc.MaxAnisotropy = 2;
    sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampler_desc.MinLOD = 0;
    sampler_desc.MaxLOD = D3D11_FLOAT32_MAX; // unlimited mipmap levels

    result = device5->CreateSamplerState(&sampler_desc,
                                         sampler.reset());
    assert(result >= 0 && "CreateSamplerState");

    // shadow mapping
    D3D11_SAMPLER_DESC sm_sampler_desc;
    ZeroMemory(&sm_sampler_desc, sizeof(sm_sampler_desc));
    sm_sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
    sm_sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
    sm_sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
    sm_sampler_desc.BorderColor[0] = 1.0f;
    sm_sampler_desc.BorderColor[1] = 1.0f;
    sm_sampler_desc.BorderColor[2] = 1.0f;
    sm_sampler_desc.BorderColor[3] = 1.0f;
    sm_sampler_desc.MinLOD = 0;
    sm_sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
    sm_sampler_desc.MipLODBias = 0;
    sm_sampler_desc.MaxAnisotropy = 0;
    sm_sampler_desc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
    sm_sampler_desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;

    result = device5->CreateSamplerState(&sm_sampler_desc,
                                         shadow_mapping_sampler.reset());
    assert(result >= 0 && "CreateSamplerState");
}

void Globals::bindSampler()
{
    device_context4->PSSetSamplers(0,
                                   1,
                                   sampler.get());
}

void Globals::initRasterizers()
{
    HRESULT result;
    
    D3D11_RASTERIZER_DESC one_sided_raster_desc;
    one_sided_raster_desc.FillMode = D3D11_FILL_SOLID;
    one_sided_raster_desc.CullMode = D3D11_CULL_BACK;
    one_sided_raster_desc.FrontCounterClockwise = false;
    one_sided_raster_desc.DepthBias = 0;
    one_sided_raster_desc.SlopeScaledDepthBias = 0.0f;
    one_sided_raster_desc.DepthBiasClamp = 0.0f;
    one_sided_raster_desc.DepthClipEnable = true;
    one_sided_raster_desc.ScissorEnable = false;
    one_sided_raster_desc.MultisampleEnable = true;
    one_sided_raster_desc.AntialiasedLineEnable = false;

    result = device->CreateRasterizerState(&one_sided_raster_desc,
                                           one_sided_rasterizer.reset());
    assert(result >= 0 && "CreateRasterizerState");
        
    D3D11_RASTERIZER_DESC double_sided_raster_desc;
    double_sided_raster_desc.FillMode = D3D11_FILL_SOLID;
    double_sided_raster_desc.CullMode = D3D11_CULL_NONE;
    double_sided_raster_desc.FrontCounterClockwise = false;
    double_sided_raster_desc.DepthBias = 0;
    double_sided_raster_desc.SlopeScaledDepthBias = 0.0f;
    double_sided_raster_desc.DepthBiasClamp = 0.0f;
    double_sided_raster_desc.DepthClipEnable = true;
    double_sided_raster_desc.ScissorEnable = false;
    double_sided_raster_desc.MultisampleEnable = true;
    double_sided_raster_desc.AntialiasedLineEnable = false;

    result = device->CreateRasterizerState(&double_sided_raster_desc,
                                           double_sided_rasterizer.reset());
    assert(result >= 0 && "CreateRasterizerState");    
}

void Globals::bindRasterizer(bool is_double_sided)
{
    if (is_double_sided) device_context4->RSSetState(double_sided_rasterizer.ptr());
    else device_context4->RSSetState(one_sided_rasterizer.ptr());
}

void Globals::initPerFrameBuffer()
{
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

void Globals::setPerFrameBuffer(const Camera & camera,
                                float EV_100)
{
    LightSystem * light_system = LightSystem::getInstance();
    TransformSystem * trans_system = TransformSystem::getInstance();
    
    // find CS corner points in WS
    glm::vec3 bottom_left_WS = camera.reproject(-1.0f, -1.0f);
    glm::vec3 top_left_WS = camera.reproject(-1.0f, 1.0f);
    glm::vec3 bottom_right_WS = camera.reproject(1.0f, -1.0f);

    // fill const buffer data
    per_frame_buffer_data.g_proj_view = camera.getViewProj();
    per_frame_buffer_data.g_camera_pos = camera.getPosition();
    per_frame_buffer_data.g_EV_100 = EV_100;
    per_frame_buffer_data.g_frustum_corners[0] = glm::vec4(bottom_left_WS, 1.0f);
    per_frame_buffer_data.g_frustum_corners[1] = glm::vec4(top_left_WS, 1.0f);
    per_frame_buffer_data.g_frustum_corners[2] = glm::vec4(bottom_right_WS, 1.0f);

    auto & point_lights = light_system->getPointLights();
    for (uint32_t size = point_lights.size(), i = 0; i != size; ++i)
    {
        uint32_t transform_id = point_lights[i].transform_id;
        
        per_frame_buffer_data.g_point_lights[i].position =
            trans_system->transforms[transform_id].position;

        per_frame_buffer_data.g_point_lights[i].radiance =
            point_lights[i].radiance;

        per_frame_buffer_data.g_point_lights[i].radius =
            point_lights[i].radius;

        // shadow map
        std::vector<Camera> cameras = Camera::generateCubemapCameras(
            trans_system->transforms[transform_id].position);

        for (uint32_t c_size = cameras.size(), c = 0; c != c_size; ++c)
        {
            per_frame_buffer_data.g_light_proj_view[6 * i + c] =
                cameras[c].getViewProj();
        }
    }

    auto & directional_lights = light_system->getDirectionalLights();
    for (uint32_t size = directional_lights.size(), i = 0; i != size; ++i)
    {
        per_frame_buffer_data.g_dir_lights[i].direction =
            directional_lights[i].direction;

        per_frame_buffer_data.g_dir_lights[i].radiance =
            directional_lights[i].radiance;

        per_frame_buffer_data.g_dir_lights[i].solid_angle =
            directional_lights[i].solid_angle;
    }
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

    device_context4->GSSetConstantBuffers(0,
                                          1,
                                          per_frame_buffer.get());
    
    device_context4->PSSetConstantBuffers(0,
                                          1,
                                          per_frame_buffer.get());
}

void Globals::initPerMeshBuffer()
{
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

void Globals::setPerMeshBuffer(const glm::mat4 & g_mesh_to_model,
                               bool g_has_albedo_texture,
                               bool g_has_roughness_texture,
                               bool g_has_metalness_texture,
                               bool g_has_normal_map,
                               bool g_is_directx_style_normal_map,
                               const glm::vec3 & g_albedo_default,
                               float g_roughness_default,
                               float g_metalness_default)
{    
    // fill const buffer data
    per_mesh_buffer_data.g_mesh_to_model = g_mesh_to_model;

    per_mesh_buffer_data.g_has_albedo_texture = g_has_albedo_texture;
    per_mesh_buffer_data.g_has_roughness_texture = g_has_roughness_texture;
    per_mesh_buffer_data.g_has_metalness_texture = g_has_metalness_texture;
    per_mesh_buffer_data.g_has_normal_map = g_has_normal_map;
    per_mesh_buffer_data.g_is_directx_style_normal_map =
        g_is_directx_style_normal_map;
    
    per_mesh_buffer_data.g_albedo_default = g_albedo_default;
    per_mesh_buffer_data.g_roughness_default = g_roughness_default;
    per_mesh_buffer_data.g_metalness_default = g_metalness_default;
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

    device_context4->PSSetConstantBuffers(1,
                                          1,
                                          per_mesh_buffer.get());
}

void Globals::initPerEmissiveMeshBuffer()
{
    D3D11_BUFFER_DESC cb_desc;
    cb_desc.Usage = D3D11_USAGE_DYNAMIC;
    cb_desc.ByteWidth = sizeof(per_emissive_mesh_buffer_data);
    cb_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cb_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cb_desc.MiscFlags = 0;
    cb_desc.StructureByteStride = 0;
    
    HRESULT result = device5->CreateBuffer(&cb_desc,
                                           NULL,
                                           per_emissive_mesh_buffer.reset());
    assert(result >= 0 && "CreateBuffer");
}

void Globals::setPerEmissiveMeshBuffer(const glm::mat4 & g_mesh_to_model)
{
    // fill const buffer data
    per_emissive_mesh_buffer_data.g_mesh_to_model = g_mesh_to_model;
}

void Globals::updatePerEmissiveMeshBuffer()
{
    HRESULT result;

    // write new data to const buffer
    D3D11_MAPPED_SUBRESOURCE ms;
    result = device_context4->Map(per_emissive_mesh_buffer.ptr(),
                                  NULL,
                                  D3D11_MAP_WRITE_DISCARD,
                                  NULL,
                                  &ms);
    assert(result >= 0 && "Map");

    memcpy(ms.pData,
           &per_emissive_mesh_buffer_data,
           sizeof(per_emissive_mesh_buffer_data));

    device_context4->Unmap(per_emissive_mesh_buffer.ptr(), NULL);

    // bind const buffer with updated values to vertex shader
    device_context4->VSSetConstantBuffers(2,
                                          1,
                                          per_emissive_mesh_buffer.get());

    device_context4->PSSetConstantBuffers(2,
                                          1,
                                          per_emissive_mesh_buffer.get());
}

void Globals::initPerShadowMeshBuffer()
{
    D3D11_BUFFER_DESC cb_desc;
    cb_desc.Usage = D3D11_USAGE_DYNAMIC;
    cb_desc.ByteWidth = sizeof(per_shadow_mesh_buffer_data);
    cb_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cb_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cb_desc.MiscFlags = 0;
    cb_desc.StructureByteStride = 0;
    
    HRESULT result = device5->CreateBuffer(&cb_desc,
                                           NULL,
                                           per_shadow_mesh_buffer.reset());
    assert(result >= 0 && "CreateBuffer");
}

void Globals::setPerShadowMeshBuffer(const glm::mat4 & g_mesh_to_model)
{
    // fill const buffer data
    per_shadow_mesh_buffer_data.g_mesh_to_model = g_mesh_to_model;
}

void Globals::updatePerShadowMeshBuffer()
{
    HRESULT result;

    // write new data to const buffer
    D3D11_MAPPED_SUBRESOURCE ms;
    result = device_context4->Map(per_shadow_mesh_buffer.ptr(),
                                  NULL,
                                  D3D11_MAP_WRITE_DISCARD,
                                  NULL,
                                  &ms);
    assert(result >= 0 && "Map");

    memcpy(ms.pData,
           &per_shadow_mesh_buffer_data,
           sizeof(per_shadow_mesh_buffer_data));

    device_context4->Unmap(per_shadow_mesh_buffer.ptr(), NULL);

    // bind const buffer with updated values to vertex shader
    device_context4->VSSetConstantBuffers(3,
                                          1,
                                          per_shadow_mesh_buffer.get());
}

void Globals::initPerShadowCubeMapBuffer()
{
    D3D11_BUFFER_DESC cb_desc;
    cb_desc.Usage = D3D11_USAGE_DYNAMIC;
    cb_desc.ByteWidth = sizeof(per_shadow_cubemap_buffer_data);
    cb_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cb_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cb_desc.MiscFlags = 0;
    cb_desc.StructureByteStride = 0;
    
    HRESULT result = device5->CreateBuffer(&cb_desc,
                                           NULL,
                                           per_shadow_cubemap_buffer.reset());
    assert(result >= 0 && "CreateBuffer");
}

void Globals::setPerShadowCubeMapBuffer(int g_cubemap_index)
{
    // fill const buffer data
    per_shadow_cubemap_buffer_data.g_cubemap_index = g_cubemap_index;
}

void Globals::updatePerShadowCubeMapBuffer()
{
    HRESULT result;

    // write new data to const buffer
    D3D11_MAPPED_SUBRESOURCE ms;
    result = device_context4->Map(per_shadow_cubemap_buffer.ptr(),
                                  NULL,
                                  D3D11_MAP_WRITE_DISCARD,
                                  NULL,
                                  &ms);
    assert(result >= 0 && "Map");

    memcpy(ms.pData,
           &per_shadow_cubemap_buffer_data,
           sizeof(per_shadow_cubemap_buffer_data));

    device_context4->Unmap(per_shadow_cubemap_buffer.ptr(), NULL);

    // bind const buffer with updated values to vertex shader
    device_context4->VSSetConstantBuffers(4,
                                          1,
                                          per_shadow_cubemap_buffer.get());

    device_context4->GSSetConstantBuffers(4,
                                          1,
                                          per_shadow_cubemap_buffer.get());
}
} // namespace engine
