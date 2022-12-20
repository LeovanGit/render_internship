#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <vector>

#include "window.hpp"
#include "camera.hpp"
#include "post_process.hpp"
#include "globals.hpp"
#include "texture_manager.hpp"
#include "shader_manager.hpp"
#include "model_manager.hpp"
#include "sky.hpp"
#include "mesh_system.hpp"
#include "particle_system.hpp"
#include "grass_system.hpp"
#include "decal_system.hpp"

namespace engine
{
class Renderer
{
public:
    Renderer() = default;

    void init(const windows::Window & window);

    void renderFrame(windows::Window & window,
                     const Camera & camera,
                     engine::Postprocess & post_process,
                     float delta_time);

    void initDepthBuffer(int width, int heigth);
    void clearDepthBuffer();
    void bindDepthBuffer();
    void changeDepthBufferAccess(bool is_read_only = false);
    void copyDepthBuffer();
    void fillDepthBufferFromCopy();

    void setStencilTest(bool is_read_only = false);
    void disableStencilTest();

    void initRenderTargetHDR(int width, int height);
    void clearRenderTargetHDR();
    void bindRenderTargetHDR(bool bind_depth_buffer = true);

    void initRenderTargetLDR(int width, int height);
    void clearRenderTargetLDR();
    
    void initGBuffer(int width, int height);
    void bindGBufferSRV();
    void bindGBufferRTV(bool bind_depth_buffer = true,
                        bool bind_model_id = true);
    void clearGBuffer();

    void unbindSRV(int slot);
    void unbindSRVs();
    void unbindRTVs();
        
    Sky sky;

    std::shared_ptr<Shader> deferred_shader;

    std::shared_ptr<Texture> reflectance;
    std::shared_ptr<Texture> irradiance;
    std::shared_ptr<Texture> reflection;
    
private:
    DxResPtr<ID3D11Texture2D> depth;
    DxResPtr<ID3D11DepthStencilView> depth_dsv;
    DxResPtr<ID3D11DepthStencilState> depth_dss;

    // copy of depth_stencil_buffer because impossible to use
    // BIND_SHADER_RESOURCE and BIND_DEPTH_STENCIL flags at the same time
    DxResPtr<ID3D11Texture2D> depth_copy;
    DxResPtr<ID3D11ShaderResourceView> depth_copy_srv;

    DxResPtr<ID3D11Texture2D> normals;
    DxResPtr<ID3D11RenderTargetView> normals_rtv;
    DxResPtr<ID3D11ShaderResourceView> normals_srv;

    DxResPtr<ID3D11Texture2D> normals_copy;
    DxResPtr<ID3D11ShaderResourceView> normals_copy_srv;
    
    DxResPtr<ID3D11Texture2D> albedo;
    DxResPtr<ID3D11RenderTargetView> albedo_rtv;
    DxResPtr<ID3D11ShaderResourceView> albedo_srv;

    DxResPtr<ID3D11Texture2D> roughness_metalness;
    DxResPtr<ID3D11RenderTargetView> roughness_metalness_rtv;
    DxResPtr<ID3D11ShaderResourceView> roughness_metalness_srv;

    DxResPtr<ID3D11Texture2D> emissive_ao;
    DxResPtr<ID3D11RenderTargetView> emissive_ao_rtv;
    DxResPtr<ID3D11ShaderResourceView> emissive_ao_srv;    

    DxResPtr<ID3D11Texture2D> model_id;
    DxResPtr<ID3D11RenderTargetView> model_id_rtv;
    DxResPtr<ID3D11ShaderResourceView> model_id_srv;
    
    DxResPtr<ID3D11Texture2D> hdr_texture;
    DxResPtr<ID3D11RenderTargetView> hdr_rtv;
    DxResPtr<ID3D11ShaderResourceView> hdr_srv;

    DxResPtr<ID3D11Texture2D> ldr_texture;
    DxResPtr<ID3D11RenderTargetView> ldr_rtv;
    DxResPtr<ID3D11ShaderResourceView> ldr_srv;
    
    void renderSceneObjects(windows::Window & window);
    void renderShadows();
    void renderParticles(float delta_time,
                         const Camera & camera);
    void renderGrass();
    void renderDecals();

    void initDepthBufferMain(int width, int height);
    void initDepthBufferCopy(int width, int height);
    
    void initNormalsTexture(int width, int height);
    void initNormalsCopy(int width, int height);
    void initAlbedoTexture(int width, int height);
    void initRoughnessMetalnessTexture(int width, int height);
    void initEmissiveTexture(int width, int height);
    void initModelIDTexture(int width, int height);

    void copyNormals();
    
    void deferredShading();
};
} // namespace engine

#endif
