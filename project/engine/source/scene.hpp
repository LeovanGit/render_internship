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

namespace engine
{
class Scene
{
public:
    Scene() = default;

    void init(const windows::Window & window);

    void renderFrame(windows::Window & window,
                     const Camera & camera,
                     engine::Postprocess & post_process);

    void initDepthBuffer(int width, int heigth);
    void clearDepthBuffer();
    void bindDepthBuffer();

    void initRenderTarget(int width, int height);
    void clearRenderTarget();
    void bindRenderTarget();

    void initShadowMap(int size);
    void clearShadowMap();
    void bindShadowMap();

    void initSquareViewport(int size);
    void bindSquareViewport();

    DxResPtr<ID3D11Texture2D> depth_stencil_buffer;
    DxResPtr<ID3D11DepthStencilView> depth_stencil_view;
    DxResPtr<ID3D11DepthStencilState> depth_stencil_state;

    DxResPtr<ID3D11Texture2D> HDR_texture;
    DxResPtr<ID3D11RenderTargetView> HDR_RTV;
    DxResPtr<ID3D11ShaderResourceView> HDR_SRV;
    
    DxResPtr<ID3D11Texture2D> shadow_map;
    DxResPtr<ID3D11DepthStencilView> shadow_map_dsv;
    DxResPtr<ID3D11DepthStencilState> shadow_map_dss;
    DxResPtr<ID3D11ShaderResourceView> shadow_map_srv;
    D3D11_VIEWPORT shadow_map_viewport;
    
    Sky sky;
};
} // namespace engine

#endif
