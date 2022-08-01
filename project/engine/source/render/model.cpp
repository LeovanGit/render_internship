#include "model.hpp"

namespace engine
{
Model::Model(const std::string & model_filename)
{
    static_assert(sizeof(glm::vec3) == sizeof(aiVector3D), "sizeof(glm::vec3)");
        
    Globals * globals = Globals::getInstance();

    // LOAD ASSIMP SCENE
    Assimp::Importer importer;
    
    uint32_t flags(aiProcess_Triangulate |
                   aiProcess_ConvertToLeftHanded);
    
    const aiScene * ai_scene = importer.ReadFile(model_filename,
                                                 flags);
    // importer.GetErrorString() for more information
    assert(ai_scene && "Assimp::Importer::ReadFile()");
   
    meshes.resize(ai_scene->mNumMeshes);    
    
    std::vector<Vertex> vertices;
    std::vector<int> indices;

    uint32_t vertex_sum = 0;
    uint32_t index_sum = 0;
    
    for (uint32_t m = 0; m != ai_scene->mNumMeshes; ++m)
    {                
        aiMesh *& src_mesh = ai_scene->mMeshes[m];
        MeshRange & dst_mesh = meshes[m];
        
        dst_mesh.vertex_count = src_mesh->mNumVertices;
        dst_mesh.index_count = src_mesh->mNumFaces * 3; // triangles

        dst_mesh.vertex_offset = vertex_sum;
        dst_mesh.index_offset = index_sum;

        vertex_sum += dst_mesh.vertex_count;
        index_sum += dst_mesh.index_count;

        // to convert from Blender (Z is up) to direct3D (Y is up) coordinates:
        aiNode * node = ai_scene->mRootNode->mChildren[m];
        dst_mesh.mesh_to_model =
            reinterpret_cast<glm::mat4 &>(node->mTransformation.Transpose());
        
        // read vertex data
        for (uint32_t v = 0; v != src_mesh->mNumVertices; ++v)
        {
            Vertex vertex;
            
            vertex.position.x = src_mesh->mVertices[v].x;
            vertex.position.y = src_mesh->mVertices[v].y;
            vertex.position.z = src_mesh->mVertices[v].z;

            // mTextureCoords[0] is main texture
            vertex.uv.x = src_mesh->mTextureCoords[0][v].x;
            vertex.uv.y = src_mesh->mTextureCoords[0][v].y;
            
            vertices.push_back(vertex);
        }

        // read index data
        for (uint32_t f = 0; f != src_mesh->mNumFaces; ++f)
        {
            aiFace & face = src_mesh->mFaces[f];

            for (uint32_t i = 0; i != face.mNumIndices; ++i)
            {
                indices.push_back(face.mIndices[i]);
            }
        }
    }
    
    vertex_buffer.init(vertices.data(), vertices.size());
    index_buffer.init(indices.data(), indices.size());
}

void Model::bind()
{
    vertex_buffer.bind(0);
    index_buffer.bind();
}

Model::MeshRange & Model::get_mesh(uint32_t index)
{
    return meshes[index];
}
} // namespace engine
