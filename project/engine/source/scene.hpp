#ifndef SCENE_HPP
#define SCENE_HPP

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

namespace engine
{
class Scene
{
public:
    Scene() = default;

    void init(const windows::Window & window);

    void renderFrame(windows::Window & window,
                     const Camera & camera,
                     engine::Postprocess & post_process,
                     float delta_time);

    void initDepthBuffer(int width, int heigth);
    void clearDepthBuffer();
    void bindDepthBuffer();
    void bindDepthBufferSRV(uint32_t slot);
    void changeDepthBufferAccess(bool is_read_only = false);
    void copyDepthBuffer();

    void initRenderTarget(int width, int height);
    void clearRenderTarget();
    void bindRenderTarget();
    
    void unbindSRV(int slot);
    void unbindRTVs();

    DxResPtr<ID3D11Texture2D> depth_texture;
    DxResPtr<ID3D11DepthStencilView> depth_dsv;
    DxResPtr<ID3D11DepthStencilState> depth_dss;

    // copy of depth_stencil_buffer because impossible to use
    // BIND_SHADER_RESOURCE and BIND_DEPTH_STENCIL flags at the same time
    DxResPtr<ID3D11Texture2D> depth_texture_copy;
    DxResPtr<ID3D11ShaderResourceView> depth_srv;

    DxResPtr<ID3D11Texture2D> depth_texture_not_ms;
    DxResPtr<ID3D11ShaderResourceView> depth_srv_not_ms;
    DxResPtr<ID3D11RenderTargetView> depth_rtv_not_ms;

    std::shared_ptr<Shader> depth_copy_shader;

    DxResPtr<ID3D11Texture2D> hdr_texture;
    DxResPtr<ID3D11RenderTargetView> hdr_rtv;
    DxResPtr<ID3D11ShaderResourceView> hdr_srv;
        
    Sky sky;
    
private:
    void renderSceneObjects(windows::Window & window);
    void renderShadows();
    void renderParticles(float delta_time,
                         const Camera & camera);
    void renderGrass();

    void initDepthBufferMS(int width, int height);
    void initDepthBufferMSCopy(int width, int height);
    void initDepthBufferNotMSCopy(int width, int height);
};
} // namespace engine

#endif
