#include "particle_system.hpp"

namespace
{
constexpr uint32_t MSAA_SAMPLES_COUNT = 4;
constexpr uint32_t SPARKS_DATA_BUFFER_SIZE = 150000;
constexpr uint32_t SPARKS_RANGE_BUFFER_SIZE = 3;
constexpr uint32_t WORKGROUP_THREADS_COUNT = 64;
} // namespace

namespace engine
{
ParticleSystem * ParticleSystem::instance = nullptr;

void ParticleSystem::init()
{
    if (!instance)
    {
        instance = new ParticleSystem();
        instance->initSparksBuffers();
    }
    else spdlog::error("ParticleSystem::init() was called twice!");
}

ParticleSystem * ParticleSystem::getInstance()
{
    return instance;
}

void ParticleSystem::del()
{
    if (instance)
    {
        delete instance;
        instance = nullptr;
    }
    else spdlog::error("ParticleSystem::del() was called twice!");
}

void ParticleSystem::addSmokeEmitter(const SmokeEmitter & smoke_emitter)
{
    smoke_emitters.push_back(smoke_emitter);
}

void ParticleSystem::updateInstanceBuffer(const Camera & camera)
{
    std::vector<Particle> instances;

    for (auto & smoke_emitter : smoke_emitters)
    {
        auto & src = smoke_emitter.getParticles();
        instances.insert(instances.end(), src.begin(), src.end());
    }
    if (instances.size() == 0) return;

    std::sort(instances.begin(),
              instances.end(),
              [camera](const Particle & a, const Particle & b)
              {
                  float dist_1 = glm::length(a.position - camera.getPosition());
                  float dist_2 = glm::length(b.position - camera.getPosition());

                  return dist_1 > dist_2;
              });

    instance_buffer.init(instances.size());
    D3D11_MAPPED_SUBRESOURCE mapped = instance_buffer.map();
    GPUInstance * dst = static_cast<GPUInstance *>(mapped.pData);

    uint32_t copied_count = 0;
    for (auto & particle : instances)
    {
        dst[copied_count++] = GPUInstance(particle.position,
                                          glm::vec3(particle.size,
                                                    particle.thickness),
                                          particle.angle,
                                          particle.tint,
                                          particle.lifetime);
    }    
    
    instance_buffer.unmap();
}

void ParticleSystem::renderParticles(float delta_time,
                                     const Camera & camera,
                                     DxResPtr<ID3D11ShaderResourceView> depth_copy_srv)
{
    for (auto & smoke_emitter : smoke_emitters)
        smoke_emitter.update(delta_time);
    
    updateInstanceBuffer(camera);

    if (instance_buffer.get_size() == 0) return;

    Globals * globals = Globals::getInstance();

    globals->bindTranslucentBlendState();
    
    globals->device_context4->
        IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    shader->bind();
    instance_buffer.bind(1);
    
    lightmap_RLT->bind(8);
    lightmap_BotBF->bind(9);
    motion_vectors->bind(10);

    globals->device_context4->PSSetShaderResources(11,
                                                   1,
                                                   depth_copy_srv.get());
    
    globals->device_context4->DrawInstanced(6,
                                            instance_buffer.get_size(),
                                            0,
                                            0);
}

void ParticleSystem::initSparksDataBuffer()
{
    HRESULT result;
    Globals * globals = Globals::getInstance();
    
    // structured buffer
    D3D11_BUFFER_DESC sb_desc;
    ZeroMemory(&sb_desc, sizeof(sb_desc));
    sb_desc.ByteWidth = SPARKS_DATA_BUFFER_SIZE * sizeof(GPUSparkData);
    sb_desc.Usage = D3D11_USAGE_DEFAULT;
    sb_desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
    sb_desc.CPUAccessFlags = 0;
    sb_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    sb_desc.StructureByteStride = sizeof(GPUSparkData);

    result = globals->device5->CreateBuffer(&sb_desc,
                                            nullptr,
                                            sparks_data.reset());
    assert(result >= 0 && "CreateBuffer");

    D3D11_UNORDERED_ACCESS_VIEW_DESC uav_desc;
    ZeroMemory(&uav_desc, sizeof(uav_desc));
    uav_desc.Format = DXGI_FORMAT_UNKNOWN;
    uav_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    uav_desc.Buffer.NumElements = SPARKS_DATA_BUFFER_SIZE;
    uav_desc.Buffer.FirstElement = 0;
    uav_desc.Buffer.Flags = 0;

    result = globals->device5->CreateUnorderedAccessView(sparks_data.ptr(),
                                                         &uav_desc,
                                                         sparks_data_view.reset());
    assert(result >= 0 && "CreateUnorderedAccessView");
}

void ParticleSystem::initSparksRangeBuffer()
{
    HRESULT result;
    Globals * globals = Globals::getInstance();
    
    D3D11_BUFFER_DESC sb_desc;
    ZeroMemory(&sb_desc, sizeof(sb_desc));
    sb_desc.ByteWidth = SPARKS_RANGE_BUFFER_SIZE * DXGI_FORMAT_R32_UINT;
    sb_desc.Usage = D3D11_USAGE_DEFAULT;
    sb_desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
    sb_desc.CPUAccessFlags = 0;
    sb_desc.MiscFlags = 0;
    sb_desc.StructureByteStride = 0;

    uint32_t data[3] = {0, 0, 0};
    D3D11_SUBRESOURCE_DATA sub_data;
    ZeroMemory(&sub_data, sizeof(sub_data));
    sub_data.pSysMem = &data;
    
    result = globals->device5->CreateBuffer(&sb_desc,
                                            &sub_data,
                                            sparks_range.reset());
    assert(result >= 0 && "CreateBuffer");

    D3D11_UNORDERED_ACCESS_VIEW_DESC uav_desc;
    ZeroMemory(&uav_desc, sizeof(uav_desc));
    uav_desc.Format = DXGI_FORMAT_R32_UINT;
    uav_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    uav_desc.Buffer.NumElements = SPARKS_RANGE_BUFFER_SIZE;
    uav_desc.Buffer.FirstElement = 0;
    uav_desc.Buffer.Flags = 0;

    result = globals->device5->CreateUnorderedAccessView(sparks_range.ptr(),
                                                         &uav_desc,
                                                         sparks_range_view.reset());
    assert(result >= 0 && "CreateUnorderedAccessView");
}

void ParticleSystem::initSparksIndirectBuffer()
{
    HRESULT result;
    Globals * globals = Globals::getInstance();
    
    // indirect arguments buffer
    D3D11_BUFFER_DESC iab_desc;
    ZeroMemory(&iab_desc, sizeof(iab_desc));
    iab_desc.ByteWidth = sizeof(D3D11_DRAW_INSTANCED_INDIRECT_ARGS);
    iab_desc.Usage = D3D11_USAGE_DEFAULT;
    iab_desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
    iab_desc.CPUAccessFlags = 0;
    iab_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    iab_desc.StructureByteStride = sizeof(D3D11_DRAW_INSTANCED_INDIRECT_ARGS);

    result = globals->device5->CreateBuffer(&iab_desc,
                                            nullptr,
                                            sparks_indirect_args.reset());
    assert(result >= 0 && "CreateBuffer");

    D3D11_UNORDERED_ACCESS_VIEW_DESC uav_desc;
    ZeroMemory(&uav_desc, sizeof(uav_desc));
    uav_desc.Format = DXGI_FORMAT_UNKNOWN;
    uav_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    uav_desc.Buffer.NumElements = 1;
    uav_desc.Buffer.FirstElement = 0;
    uav_desc.Buffer.Flags = 0;

    result = globals->device5->CreateUnorderedAccessView(sparks_indirect_args.ptr(),
                                                         &uav_desc,
                                                         sparks_indirect_args_view.reset());
    assert(result >= 0 && "CreateUnorderedAccessView");
}

void ParticleSystem::initSparksIndirectBufferCopy()
{
    HRESULT result;
    Globals * globals = Globals::getInstance();
    
    // indirect arguments buffer
    D3D11_BUFFER_DESC iab_desc;
    ZeroMemory(&iab_desc, sizeof(iab_desc));
    iab_desc.ByteWidth = sizeof(D3D11_DRAW_INSTANCED_INDIRECT_ARGS);
    iab_desc.Usage = D3D11_USAGE_DEFAULT;
    iab_desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
    iab_desc.CPUAccessFlags = 0;
    iab_desc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
    iab_desc.StructureByteStride = sizeof(D3D11_DRAW_INSTANCED_INDIRECT_ARGS);
    
    result = globals->device5->CreateBuffer(&iab_desc,
                                            nullptr,
                                            sparks_indirect_args_copy.reset());
    assert(result >= 0 && "CreateBuffer");
}

void ParticleSystem::initSparksBuffers()
{
    initSparksDataBuffer();
    initSparksRangeBuffer();
    initSparksIndirectBuffer();
    initSparksIndirectBufferCopy();
}

void ParticleSystem::copySparksIndirectBuffer()
{
    Globals * globals = Globals::getInstance();

    globals->device_context4->CopyResource(sparks_indirect_args_copy.ptr(),
                                           sparks_indirect_args.ptr());
}

void ParticleSystem::bindSparksBuffers(bool to_compute_shader)
{
    Globals * globals = Globals::getInstance();
    
    ID3D11UnorderedAccessView * uavs[] = {sparks_data_view.ptr(),
                                          sparks_range_view.ptr(),
                                          sparks_indirect_args_view.ptr()};

    if (to_compute_shader)
    {
        globals->device_context4->CSSetUnorderedAccessViews(1,
                                                            3,
                                                            uavs,
                                                            nullptr);
        return;
    }
        
    globals->device_context4->
        OMSetRenderTargetsAndUnorderedAccessViews(D3D11_KEEP_RENDER_TARGETS_AND_DEPTH_STENCIL,
                                                  nullptr,
                                                  nullptr,
                                                  1,
                                                  3,
                                                  uavs,
                                                  nullptr);
}

void ParticleSystem::unbindSparksBuffers()
{
    Globals * globals = Globals::getInstance();
    
    ID3D11UnorderedAccessView * uavs[] = {nullptr,
                                          nullptr,
                                          nullptr};
    
    globals->device_context4->
        OMSetRenderTargetsAndUnorderedAccessViews(D3D11_KEEP_RENDER_TARGETS_AND_DEPTH_STENCIL,
                                                  nullptr,
                                                  nullptr,
                                                  1,
                                                  3,
                                                  uavs,
                                                  nullptr);
}

void ParticleSystem::spawnSparks()
{
    MeshSystem * mesh_sys = MeshSystem::getInstance();
    
    bindSparksBuffers();
    spawn_sparks->bind();

    mesh_sys->disappear_instances.renderWithoutMaterials();
    
    unbindSparksBuffers();
}

void ParticleSystem::updateSparks(DxResPtr<ID3D11ShaderResourceView> depth_copy_srv,
                                  DxResPtr<ID3D11ShaderResourceView> normals_copy_srv)
{
    Globals * globals = Globals::getInstance();
    
    bindSparksBuffers(true);

    update_ring_buffer->bind();
    globals->device_context4->Dispatch(1, 1, 1);

    update_sparks->bind();

    globals->device_context4->CSSetShaderResources(0,
                                                   1,
                                                   depth_copy_srv.get());

    globals->device_context4->CSSetShaderResources(1,
                                                   1,
                                                   normals_copy_srv.get());
    
    uint32_t work_groups_count = SPARKS_DATA_BUFFER_SIZE / WORKGROUP_THREADS_COUNT + 1;
    globals->device_context4->Dispatch(work_groups_count, 1, 1);

    unbindSparksBuffers();
}

void ParticleSystem::drawSparks(DxResPtr<ID3D11ShaderResourceView> depth_copy_srv)
{
    Globals * globals = Globals::getInstance();

    copySparksIndirectBuffer();
    bindSparksBuffers();
    
    render_sparks->bind();
    spark->bind(0);

    globals->device_context4->CSSetShaderResources(1,
                                                   1,
                                                   depth_copy_srv.get());
    
    globals->bindTranslucentBlendState();

    globals->device_context4->DrawInstancedIndirect(sparks_indirect_args_copy.ptr(),
                                                    0);

    unbindSparksBuffers();
}

void ParticleSystem::renderSparks(DxResPtr<ID3D11ShaderResourceView> depth_copy_srv,
                                  DxResPtr<ID3D11ShaderResourceView> normals_copy_srv)
{    
    spawnSparks();
    updateSparks(depth_copy_srv, normals_copy_srv);
    drawSparks(depth_copy_srv);
}
} // namespace engine

