#ifndef INDEX_BUFFER_HPP
#define INDEX_BUFFER_HPP

#include <d3d11_4.h>
#include <cassert>
#include <array>

#include "dx_res_ptr.hpp"
#include "globals.hpp"

namespace engine
{
class IndexBuffer
{    
public:
    IndexBuffer() = default;
    
    void init(int indices[],
              uint32_t size);

    DxResPtr<ID3D11Buffer> & get_data() { return data; }
    
private:
    DxResPtr<ID3D11Buffer> data;
};
} // namespace engine

#endif
