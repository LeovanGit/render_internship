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
                   aiProcess_ConvertToLeftHanded |
                   aiProcess_CalcTangentSpace |
                   aiProcess_GenBoundingBoxes);
    
    const aiScene * ai_scene = importer.ReadFile(model_filename,
                                                 flags);
    // importer.GetErrorString() for more information
    assert(ai_scene && "Assimp::Importer::ReadFile()");
   
    meshes.resize(ai_scene->mNumMeshes);
    octrees.resize(ai_scene->mNumMeshes);
    
    std::vector<Vertex> vertices;
    std::vector<int> indices;
    
    uint32_t vertex_sum = 0;
    uint32_t index_sum = 0;

    // model bounding box
    box.reset();
    
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

        // for collision in TransfromSystem
        math::Mesh mesh;
        mesh.box.min = reinterpret_cast<glm::vec3 &>(src_mesh->mAABB.mMin);
        mesh.box.max = reinterpret_cast<glm::vec3 &>(src_mesh->mAABB.mMax);

        if (mesh.box.min.x < box.min.x) box.min.x = mesh.box.min.x;
        if (mesh.box.min.y < box.min.y) box.min.y = mesh.box.min.y;
        if (mesh.box.min.z < box.min.z) box.min.z = mesh.box.min.z;

        if (mesh.box.max.x > box.max.x) box.max.x = mesh.box.max.x;
        if (mesh.box.max.y > box.max.y) box.max.y = mesh.box.max.y;
        if (mesh.box.max.z > box.max.z) box.max.z = mesh.box.max.z;
        
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

            vertex.normal = reinterpret_cast<glm::vec3 &>(src_mesh->mNormals[v]);
            vertex.tangent = reinterpret_cast<glm::vec3 &>(src_mesh->mTangents[v]);
            // flip
            vertex.bitangent = reinterpret_cast<glm::vec3 &>(src_mesh->mBitangents[v]);
               
            vertices.push_back(vertex);
            mesh.vertices.push_back(vertex);
        }

        mesh.triangles.resize(src_mesh->mNumFaces);

        // read index data
        for (uint32_t f = 0; f != src_mesh->mNumFaces; ++f)
        {
            aiFace & face = src_mesh->mFaces[f];

            for (uint32_t i = 0; i != face.mNumIndices; ++i)
            {
                indices.push_back(face.mIndices[i]);
                mesh.triangles[f].indices[i] = face.mIndices[i];
            }
        }
        octrees[m].initialize(std::make_shared<math::Mesh>(mesh));
    }
    
    vertex_buffer.init(vertices.data(), vertices.size());
    index_buffer.init(indices.data(), indices.size());
}

// for generate default objects with one mesh (sphere, cube, plane)
Model::Model(std::vector<Vertex> & vertices,
             std::vector<int> & indices)
{
    uint32_t vertices_size = vertices.size();
    uint32_t indices_size = indices.size();
    
    vertex_buffer.init(vertices.data(), vertices_size);
    index_buffer.init(indices.data(), indices_size);

    glm::mat4 mesh_to_model(1.0f, 0.0f, 0.0f, 0.0f,
                            0.0f, 1.0f, 0.0f, 0.0f,
                            0.0f, 0.0f, 1.0f, 0.0f,
                            0.0f, 0.0f, 0.0f, 1.0f);
    
    MeshRange mesh_range {vertices_size,
                          indices_size,
                          0,
                          0,
                          mesh_to_model};

    meshes.push_back(mesh_range);

    math::Mesh mesh;
    mesh.vertices = vertices;
    mesh.box = math::BoundingBox::unit();
    box = mesh.box;
    for (uint32_t i = 0, size = indices_size; i != size; i += 3)
    {
        math::Mesh::Triangle triangle;
        triangle.indices[0] = indices[i];
        triangle.indices[1] = indices[i + 1];
        triangle.indices[2] = indices[i + 2];
        
        mesh.triangles.push_back(triangle);
    }
    
    octrees.resize(1);
    octrees[0].initialize(std::make_shared<math::Mesh>(mesh));
}

void Model::bind()
{
    vertex_buffer.bind(0);
    index_buffer.bind();
}

std::vector<Model::MeshRange> & Model::getMeshRanges()
{
    return meshes;
}

Model::MeshRange & Model::getMeshRange(uint32_t index)
{
    return meshes[index];
}

std::vector<math::TriangleOctree> & Model::getOctree()
{
    return octrees;
}

math::BoundingBox Model::getBox()
{
    return box;
}
} // namespace engine
