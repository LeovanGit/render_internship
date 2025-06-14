#include "index_buffer.hpp"

namespace engine
{
void IndexBuffer::init(int indices[],
                       uint32_t size)
{
    Globals * globals = Globals::getInstance();

    D3D11_BUFFER_DESC ibo_desc;
    ZeroMemory(&ibo_desc, sizeof(ibo_desc));
    ibo_desc.Usage = D3D11_USAGE_IMMUTABLE;
    ibo_desc.ByteWidth = sizeof(int) * size;
    ibo_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibo_desc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA indices_data;
    ZeroMemory(&indices_data, sizeof(indices_data));
    indices_data.pSysMem = indices;
    indices_data.SysMemPitch = 0;
    indices_data.SysMemSlicePitch = 0;

    HRESULT result = globals->device5->CreateBuffer(&ibo_desc,
                                                    &indices_data,
                                                    data.reset());
    assert(result >= 0 && "CreateBuffer(index)");

    this->size = size;
}

void IndexBuffer::bind()
{
    Globals * globals = Globals::getInstance();

    globals->device_context4->IASetIndexBuffer(data.ptr(),
                                               DXGI_FORMAT_R32_UINT,
                                               0);
}

const DxResPtr<ID3D11Buffer> & IndexBuffer::get_data() const
{
    return data;
}

uint32_t IndexBuffer::get_size() const
{
    return size;
}

} // namespace engine
