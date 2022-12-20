#ifndef POST_PROCESS_HPP
#define POST_PROCESS_HPP

#include "globals.hpp"
#include "shader_manager.hpp"

namespace engine
{
class Postprocess
{
public:
    Postprocess() = default;

    void init(std::shared_ptr<Shader> resolve_shader,
              std::shared_ptr<Shader> blur_shader,
              float EV_100 = 0.0f);
    
    void resolve(DxResPtr<ID3D11ShaderResourceView> source_hdr,
                 DxResPtr<ID3D11RenderTargetView> target_ldr);

    void gaussian_blur(DxResPtr<ID3D11ShaderResourceView> source,
                       DxResPtr<ID3D11RenderTargetView> target);
    
    float EV_100;
    std::shared_ptr<Shader> resolve_shader;
    std::shared_ptr<Shader> blur_shader;
};
} // namespace engine

#endif

