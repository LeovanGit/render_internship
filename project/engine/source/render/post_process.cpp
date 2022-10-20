#include "post_process.hpp"

namespace engine
{
void Postprocess::init(std::shared_ptr<Shader> shader,
                       float EV_100)
{
    this->shader = shader;
    this->EV_100 = EV_100;
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
    
    shader->bind();

    globals->device_context4->Draw(3, 0);
}
} // namespace engine
