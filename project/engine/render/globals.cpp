#include "globals.hpp"

// Say NVidia or AMD driver to prefer a dedicated GPU instead of an integrated
// This has effect on laptops
extern "C"
{
    _declspec(dllexport) uint32_t NvOptimusEnablement = 1;
    _declspec(dllexport) uint32_t AmdPowerXpressRequestHighPerformance = 1;
}

namespace engine
{
Globals * Globals::instance = nullptr;

void Globals::init()
{
    if (!instance) instance = new Globals();
}

Globals * Globals::getInstance()
{
    return instance;
}

void Globals::del()
{
    if (instance)
    {
        delete instance;
        instance = nullptr;
    }
}

void Globals::initD3D()
{
    HRESULT result;

    // FACTORY
    result = CreateDXGIFactory(__uuidof(IDXGIFactory),
                              (void **)factory.reset());
    assert(result >= 0 && "CreateDXGIFactory");

    result = factory->QueryInterface(__uuidof(IDXGIFactory5),
                                    (void**)factory5.reset());
    assert(result >= 0 && "Query IDXGIFactory5");

    // ADAPTERS (provide info about your GPUs)
    {
        uint32_t index = 0;
        IDXGIAdapter1 * adapter;

        std::cout << "\n===> AVAILABLE ADAPTERS <===\n";

        while (factory5->EnumAdapters1(index++,
                                       &adapter) != DXGI_ERROR_NOT_FOUND)
        {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);
            
            std::wcout << "GPU #" << index << " " << desc.Description << "\n";
        }

        std::cout << "============================\n";
    }

    // DEVICE AND DEVICE CONTEXT
    const D3D_FEATURE_LEVEL featureLevelRequested = D3D_FEATURE_LEVEL_11_0;
    D3D_FEATURE_LEVEL featureLevelInitialized = D3D_FEATURE_LEVEL_11_0;

    result = D3D11CreateDevice(nullptr,
                               D3D_DRIVER_TYPE_HARDWARE,
                               nullptr,
                               D3D11_CREATE_DEVICE_DEBUG,
                               &featureLevelRequested,
                               1,
                               D3D11_SDK_VERSION,
                               device.reset(),
                               &featureLevelInitialized,
                               device_context.reset());

    assert(result >= 0 && "D3D11CreateDevice");
    assert(featureLevelRequested == featureLevelInitialized &&
           "D3D_FEATURE_LEVEL_11_0");

    result = device->QueryInterface(__uuidof(ID3D11Device5),
                                   (void**)device5.reset());
    assert(result >= 0 && "Query ID3D11Device5");

    result = device_context->QueryInterface(__uuidof(ID3D11DeviceContext4), 
                                            (void**)device_context4.reset());
    assert(result >= 0 && "Query ID3D11DeviceContext4");

    result = device->QueryInterface(__uuidof(ID3D11Debug),
                                   (void**)device_debug.reset());
    assert(result >= 0 && "Query ID3D11Debug");
}

void Globals::initVBO()
{
    float obj_size = 4.0f;

    std::array<Vertex, 36> vertices =
    {
        //     POSITION                UV
        // front
        Vertex{{-obj_size, obj_size,  -obj_size}, {0.0f, 1.0f}},
        Vertex{{obj_size,  obj_size,  -obj_size}, {1.0f, 1.0f}},
        Vertex{{obj_size, -obj_size,  -obj_size}, {1.0f, 0.0f}},

        Vertex{{obj_size,  -obj_size,  -obj_size}, {1.0f, 0.0f}},
        Vertex{{-obj_size, -obj_size,  -obj_size}, {0.0f, 0.0f}},
        Vertex{{-obj_size,  obj_size,  -obj_size}, {0.0f, 1.0f}},

        // right
        Vertex{{obj_size, obj_size,  -obj_size}, {0.0f, 1.0f}},
        Vertex{{obj_size, obj_size,   obj_size}, {1.0f, 1.0f}},
        Vertex{{obj_size, -obj_size, -obj_size}, {0.0f, 0.0f}},

        Vertex{{obj_size, -obj_size, -obj_size}, {0.0f, 0.0f}},
        Vertex{{obj_size,  obj_size,  obj_size}, {1.0f, 1.0f}},
        Vertex{{obj_size, -obj_size,  obj_size}, {1.0f, 0.0f}},

        // left
        Vertex{{-obj_size,  obj_size, -obj_size}, {1.0f, 1.0f}},
        Vertex{{-obj_size, -obj_size, -obj_size}, {1.0f, 0.0f}},
        Vertex{{-obj_size,  obj_size,  obj_size}, {0.0f, 1.0f}},

        Vertex{{-obj_size,  obj_size,  obj_size}, {0.0f, 1.0f}},
        Vertex{{-obj_size, -obj_size, -obj_size}, {1.0f, 0.0f}},
        Vertex{{-obj_size, -obj_size,  obj_size}, {0.0f, 0.0f}},

        // back
        Vertex{{ obj_size,  obj_size, obj_size}, {1.0f, 1.0f}},
        Vertex{{-obj_size,  obj_size, obj_size}, {0.0f, 1.0f}},
        Vertex{{ obj_size, -obj_size, obj_size}, {1.0f, 0.0f}},

        Vertex{{ obj_size, -obj_size, obj_size}, {1.0f, 0.0f}},
        Vertex{{-obj_size,  obj_size, obj_size}, {0.0f, 1.0f}},
        Vertex{{-obj_size, -obj_size, obj_size}, {0.0f, 0.0f}},

        // top
        Vertex{{ obj_size, obj_size,  obj_size}, {1.0f, 1.0f}},
        Vertex{{ obj_size, obj_size, -obj_size}, {1.0f, 0.0f}},
        Vertex{{-obj_size, obj_size, -obj_size}, {0.0f, 0.0f}},

        Vertex{{-obj_size, obj_size, -obj_size}, {0.0f, 0.0f}},
        Vertex{{-obj_size, obj_size,  obj_size}, {0.0f, 1.0f}},
        Vertex{{ obj_size, obj_size,  obj_size}, {1.0f, 1.0f}},

        // down
        Vertex{{ obj_size, -obj_size,  obj_size}, {1.0f, 1.0f}},
        Vertex{{-obj_size, -obj_size, -obj_size}, {0.0f, 0.0f}},
        Vertex{{ obj_size, -obj_size, -obj_size}, {1.0f, 0.0f}},

        Vertex{{-obj_size, -obj_size, -obj_size}, {0.0f, 0.0f}},
        Vertex{{ obj_size, -obj_size,  obj_size}, {1.0f, 1.0f}},
        Vertex{{-obj_size, -obj_size,  obj_size}, {0.0f, 1.0f}},
    };

    // contain properties of the VBO
    D3D11_BUFFER_DESC vbo_desc;
    ZeroMemory(&vbo_desc, sizeof(vbo_desc));

    vbo_desc.Usage = D3D11_USAGE_IMMUTABLE;
    vbo_desc.ByteWidth = sizeof(Vertex) * vertices.size();
    vbo_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbo_desc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA vertices_data;
    ZeroMemory(&vertices_data, sizeof(vertices_data));

    vertices_data.pSysMem = vertices.data();
    vertices_data.SysMemPitch = 0;
    vertices_data.SysMemSlicePitch = 0;

    HRESULT result = device5->CreateBuffer(&vbo_desc,
                                           &vertices_data,
                                           vbo.reset());
    assert(result >= 0 && "CreateBuffer");
}

void Globals::setConstBuffer(const Camera & camera)
{
    // fill const buffer data
    const_buffer_data.proj_view = camera.getViewProj();

    if (const_buffer.valid()) return;
    // constant buffer description
    D3D11_BUFFER_DESC cb_desc;
    cb_desc.Usage = D3D11_USAGE_DYNAMIC;
    cb_desc.ByteWidth = sizeof(const_buffer_data);
    cb_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cb_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cb_desc.MiscFlags = 0;
    cb_desc.StructureByteStride = 0;
    
    HRESULT result = device5->CreateBuffer(&cb_desc,
                                           NULL,
                                           const_buffer.get());
    assert(result >= 0 && "CreateBuffer");
}
} // namespace engine
