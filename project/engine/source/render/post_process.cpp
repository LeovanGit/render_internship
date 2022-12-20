#include "post_process.hpp"

namespace engine
{
void Postprocess::init(std::shared_ptr<Shader> resolve_shader,
                       std::shared_ptr<Shader> blur_shader,
                       float EV_100)
{
    this->resolve_shader = resolve_shader;
    this->EV_100 = EV_100;

    this->blur_shader = blur_shader;
}

void Postprocess::resolve(DxResPtr<ID3D11ShaderResourceView> source_hdr,
                          DxResPtr<ID3D11RenderTargetView> target_ldr)
{
    Globals * globals = Globals::getInstance();

    globals->device_context4->OMSetRenderTargets(1,
                                                 target_ldr.get(),
                                                 NULL);

    globals->device_context4->PSSetShaderResources(0,
                                                   1,
                                                   source_hdr.get());
    
    resolve_shader->bind();

    globals->device_context4->Draw(3, 0);
}

void Postprocess::gaussian_blur(DxResPtr<ID3D11ShaderResourceView> source,
                                DxResPtr<ID3D11RenderTargetView> target)
{
    Globals * globals = Globals::getInstance();

    globals->device_context4->OMSetRenderTargets(1,
                                                 target.get(),
                                                 NULL);

    globals->device_context4->PSSetShaderResources(0,
                                                   1,
                                                   source.get());
    
    blur_shader->bind();

    globals->device_context4->Draw(3, 0);
}
} // namespace engine
