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

    const DxResPtr<ID3D11Buffer> & get_data() const;
    uint32_t get_size() const;
    
private:
    DxResPtr<ID3D11Buffer> data;
    uint32_t size;
};
} // namespace engine

#endif
