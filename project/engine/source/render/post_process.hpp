#ifndef POST_PROCESS_HPP
#define POST_PROCESS_HPP

#include "globals.hpp"
#include "shader_manager.hpp"

namespace engine
{
class Postprocess
{
public:
    Postprocess() : EV_100(-1.0f) {};
    
    void resolve(DxResPtr<ID3D11ShaderResourceView> source_HDR,
                 DxResPtr<ID3D11RenderTargetView> target_LDR);
    
    float EV_100;
};
} // namespace engine

#endif

