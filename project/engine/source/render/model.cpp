#include "model.hpp"

namespace engine
{
Model::Model(const std::string & model_filename)
{
    Globals * globals = Globals::getInstance();

    // LOAD ASSIMP SCENE
    Assimp::Importer importer;
    
    uint32_t flags(aiProcess_Triangulate |
                   aiProcess_GenBoundingBoxes |
                   aiProcess_ConvertToLeftHanded |
                   aiProcess_CalcTangentSpace);
    
    const aiScene * ai_scene = importer.ReadFile(model_filename,
                                              flags);
    // importer.GetErrorString() for more information
    assert(ai_scene && "Assimp::Importer::ReadFile()");

    int num_meshes = ai_scene->mNumMeshes;

    // LOAD VERTEX DATA
    for (int i = 0; i != num_meshes; ++i)
    {

    }
    
    float obj_size = 4.0f;

    std::array<Vertex, 36> vertices =
    {
        //     POSITION                           UV
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

    HRESULT result = globals->device5->CreateBuffer(&vbo_desc,
                                                    &vertices_data,
                                                    vertex_buffer.reset());
    assert(result >= 0 && "CreateBuffer");    
}

void Model::bind()
{
    Globals * globals = Globals::getInstance();
    
    uint32_t stride = sizeof(Vertex);
    uint32_t offset = 0;
    globals->device_context4->IASetVertexBuffers(0,
                                                 1,
                                                 vertex_buffer.get(),
                                                 &stride,
                                                 &offset);
}
} // namespace engine
