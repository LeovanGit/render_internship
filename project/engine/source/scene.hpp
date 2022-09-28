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
    void changeDepthBufferAccess(bool is_read_only = false);

    void initRenderTarget(int width, int height);
    void clearRenderTarget();
    void bindRenderTarget();
    
    void unbindSRV(int slot);

    DxResPtr<ID3D11Texture2D> depth_stencil_buffer;
    DxResPtr<ID3D11DepthStencilView> depth_stencil_view;
    DxResPtr<ID3D11DepthStencilState> depth_stencil_state;

    DxResPtr<ID3D11Texture2D> HDR_texture;
    DxResPtr<ID3D11RenderTargetView> HDR_RTV;
    DxResPtr<ID3D11ShaderResourceView> HDR_SRV;
        
    Sky sky;
    
private:
    void renderSceneObjects(windows::Window & window);
    void renderShadows();

};
} // namespace engine

#endif
