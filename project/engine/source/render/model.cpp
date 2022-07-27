#include "model.hpp"

namespace engine
{
Model::Model(const std::string & model_filename)
{
    Globals * globals = Globals::getInstance();

    // // LOAD ASSIMP SCENE
    // Assimp::Importer importer;
    
    // uint32_t flags(aiProcess_Triangulate |
    //                aiProcess_GenBoundingBoxes |
    //                aiProcess_ConvertToLeftHanded |
    //                aiProcess_CalcTangentSpace);
    
    // const aiScene * ai_scene = importer.ReadFile(model_filename,
    //                                              flags);
    // // importer.GetErrorString() for more information
    // assert(ai_scene && "Assimp::Importer::ReadFile()");

    // uint32_t num_meshes = ai_scene->mNumMeshes;

    // CREATE VERTEX BUFFER
    float obj_size = 4.0f;
    Vertex vertices[] =
    {
        // UV with a little offset to disable wrapping
        //     POSITION                           UV
        Vertex{{-obj_size, -obj_size, -obj_size}, {0.01f, 0.99f}},
        Vertex{{-obj_size,  obj_size, -obj_size}, {0.01f, 0.01f}},
        Vertex{{ obj_size,  obj_size, -obj_size}, {0.99f, 0.01f}},
        Vertex{{ obj_size, -obj_size, -obj_size}, {0.99f, 0.99f}},
        Vertex{{ obj_size, -obj_size,  obj_size}, {0.01f, 0.99f}},
        Vertex{{ obj_size,  obj_size,  obj_size}, {0.01f, 0.01f}},
        Vertex{{-obj_size,  obj_size,  obj_size}, {0.99f, 0.01f}},
        Vertex{{-obj_size, -obj_size,  obj_size}, {0.99f, 0.99f}},

        // additional vertices for texturing top and bot
        Vertex{{ obj_size,  obj_size,  obj_size}, {0.99f, 0.99f}},
        Vertex{{-obj_size,  obj_size,  obj_size}, {0.01f, 0.99f}},
        Vertex{{ obj_size, -obj_size,  obj_size}, {0.99f, 0.01f}},
        Vertex{{-obj_size, -obj_size,  obj_size}, {0.01f, 0.01f}},
    };

    vertex_buffer.init(vertices, 12);
    
    // CREATE INDEX BUFFER
    int indices[] =
    {
        // front
        0, 1, 3,
        1, 2, 3,

        // right
        3, 2, 5,
        4, 3, 5,
        
        // top
        8, 2, 1,
        1, 9, 8,
        
        // back
        5, 6, 4,
        6, 7, 4,

        // left
        6, 1, 0,
        0, 7, 6,

        // bot
        0, 3, 10,
        0, 10, 7        
    };
    
    index_buffer.init(indices, 36);
}

void Model::bind()
{
    Globals * globals = Globals::getInstance();
    
    globals->device_context4->IASetVertexBuffers(0,
                                                 1,
                                                 vertex_buffer.get_data().get(),
                                                 &vertex_buffer.get_stride(),
                                                 &vertex_buffer.get_offset());

    globals->device_context4->IASetIndexBuffer(index_buffer.get_data().ptr(),
                                               DXGI_FORMAT_R32_UINT,
                                               0);

}
} // namespace engine
