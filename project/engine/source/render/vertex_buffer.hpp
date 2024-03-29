#ifndef VERTEX_BUFFER_HPP
#define VERTEX_BUFFER_HPP

#include <d3d11_4.h>
#include <cassert>

#include "dx_res_ptr.hpp"
#include "globals.hpp"

namespace engine
{
template <class T>
class VertexBuffer
{    
public:    
    VertexBuffer() = default;
    
    void init(T * vertices, uint32_t size)
    {
        Globals * globals = Globals::getInstance();

        D3D11_BUFFER_DESC vbo_desc;
        ZeroMemory(&vbo_desc, sizeof(vbo_desc));
        vbo_desc.Usage = D3D11_USAGE_IMMUTABLE;
        vbo_desc.ByteWidth = sizeof(T) * size;
        vbo_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vbo_desc.CPUAccessFlags = 0;

        D3D11_SUBRESOURCE_DATA vertices_data;
        ZeroMemory(&vertices_data, sizeof(vertices_data));
        vertices_data.pSysMem = vertices;
        vertices_data.SysMemPitch = 0;
        vertices_data.SysMemSlicePitch = 0;

        HRESULT result = globals->device5->CreateBuffer(&vbo_desc,
                                                        &vertices_data,
                                                        data.reset());
        assert(result >= 0 && "CreateBuffer()");

        this->size = size;
        stride = sizeof(T);
        offset = 0;
    }

    void init(uint32_t size)
    {
        Globals * globals = Globals::getInstance();

        D3D11_BUFFER_DESC vbo_desc;
        ZeroMemory(&vbo_desc, sizeof(vbo_desc));
        vbo_desc.Usage = D3D11_USAGE_DYNAMIC;
        vbo_desc.ByteWidth = sizeof(T) * size;
        vbo_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vbo_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        HRESULT result = globals->device5->CreateBuffer(&vbo_desc,
                                                        NULL,
                                                        data.reset());
        assert(result >= 0 && "CreateBuffer()");

        this->size = size;
        this->stride = sizeof(T);
        offset = 0;
    }

    D3D11_MAPPED_SUBRESOURCE map() const
    {
        Globals * globals = Globals::getInstance();
        
        D3D11_MAPPED_SUBRESOURCE ms;
        HRESULT result = globals->device_context4->Map(data.ptr(),
                                                       NULL,
                                                       D3D11_MAP_WRITE_DISCARD,
                                                       NULL,
                                                       &ms);
        assert(result >= 0 && "Map");

        return ms;
    }

    void unmap() const
    {
        Globals * globals = Globals::getInstance();
        
        globals->device_context4->Unmap(data.ptr(), NULL);
    }
    
    void bind(uint32_t slot)
    {
        Globals * globals = Globals::getInstance();

        globals->device_context4->IASetVertexBuffers(slot,
                                                     1,
                                                     data.get(),
                                                     &stride,
                                                     &offset);
    }

    DxResPtr<ID3D11Buffer> & get_data() { return data; }
    uint32_t get_size() const { return size; }
    const uint32_t & get_stride() const { return stride; }
    const uint32_t & get_offset() const { return offset; }

private:
    DxResPtr<ID3D11Buffer> data;
    uint32_t size;
    uint32_t stride;
    uint32_t offset;
};
} // namespace engine

#endif
