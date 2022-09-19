#include "post_process.hpp"

namespace
{
constexpr float BACKGROUND[4] = {0.4f, 0.44f, 0.4f, 1.0f};
} // namespace

namespace engine
{
void Postprocess::init(std::shared_ptr<Shader> shader,
                       float EV_100)
{
    this->shader = shader;
    this->EV_100 = EV_100;
}

void Postprocess::resolve(DxResPtr<ID3D11ShaderResourceView> source_HDR,
                          DxResPtr<ID3D11RenderTargetView> target_LDR)
{
    Globals * globals = Globals::getInstance();

    globals->device_context4->OMSetRenderTargets(1,
                                                 target_LDR.get(),
                                                 NULL);
    globals->device_context4->ClearRenderTargetView(target_LDR.ptr(),
                                                    BACKGROUND);

    globals->device_context4->PSSetShaderResources(0,
                                                   1,
                                                   source_HDR.get());
    
    shader->bind();

    globals->device_context4->Draw(3, 0);
}
} // namespace engine
