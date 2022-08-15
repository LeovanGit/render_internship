#include "model_manager.hpp"

namespace engine
{
ModelManager * ModelManager::instance = nullptr;

void ModelManager::init()
{
    if (!instance) instance = new ModelManager();
    else spdlog::error("ModelManager::init() was called twice!");
}

ModelManager * ModelManager::getInstance()
{
    return instance;
}

void ModelManager::del()
{
    if (instance)
    {
        delete instance;
        instance = nullptr;
    }
    else spdlog::error("ModelManager::del() was called twice!");
}

std::shared_ptr<Model> ModelManager::getModel(const std::string & model_path)
{
    auto item = models.find(model_path);
    if (item != models.end()) return item->second;
    
    auto result = models.try_emplace(model_path,
                                     std::make_shared<Model>(model_path));
    return result.first->second;
}

void ModelManager::bindModel(const std::string & key)
{
    models.find(key)->second->bind();
}

std::shared_ptr<Model> ModelManager::getDefaultCube(const std::string & key)
{
    auto item = models.find(key);
    if (item != models.end()) return item->second;
    
    typedef Model::Vertex Vertex;

    // CREATE VERTEX BUFFER
    Vertex vertices[] =
    {
        // UV with a little offset to disable wrapping
        //     POSITION               UV              NORMAL               TANGENT             BITANGENT
        // front
        Vertex{{-1.0f, 1.0f,  -1.0f}, {0.01f, 0.01f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        Vertex{{1.0f,  1.0f,  -1.0f}, {0.99f, 0.01f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        Vertex{{1.0f, -1.0f,  -1.0f}, {0.99f, 0.99f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},

        Vertex{{1.0f,  -1.0f,  -1.0f}, {0.99f, 0.99f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        Vertex{{-1.0f, -1.0f,  -1.0f}, {0.01f, 0.99f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        Vertex{{-1.0f,  1.0f,  -1.0f}, {0.01f, 0.01f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},

        // right
        Vertex{{1.0f, 1.0f,  -1.0f}, {0.01f, 0.01f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
        Vertex{{1.0f, 1.0f,   1.0f}, {0.99f, 0.01f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
        Vertex{{1.0f, -1.0f, -1.0f}, {0.01f, 0.99f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},

        Vertex{{1.0f, -1.0f, -1.0f}, {0.01f, 0.99f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
        Vertex{{1.0f,  1.0f,  1.0f}, {0.99f, 0.01f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
        Vertex{{1.0f, -1.0f,  1.0f}, {0.99f, 0.99f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},

        // left
        Vertex{{-1.0f,  1.0f, -1.0f}, {0.99f, 0.01f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}},
        Vertex{{-1.0f, -1.0f, -1.0f}, {0.99f, 0.99f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}},
        Vertex{{-1.0f,  1.0f,  1.0f}, {0.01f, 0.01f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}},

        Vertex{{-1.0f,  1.0f,  1.0f}, {0.01f, 0.01f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}},
        Vertex{{-1.0f, -1.0f, -1.0f}, {0.99f, 0.99f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}},
        Vertex{{-1.0f, -1.0f,  1.0f}, {0.01f, 0.99f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}},

        // back
        Vertex{{ 1.0f,  1.0f, 1.0f}, {0.99f, 0.01f}, {0.0f, 0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        Vertex{{-1.0f,  1.0f, 1.0f}, {0.01f, 0.01f}, {0.0f, 0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        Vertex{{ 1.0f, -1.0f, 1.0f}, {0.99f, 0.99f}, {0.0f, 0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},

        Vertex{{ 1.0f, -1.0f, 1.0f}, {0.99f, 0.99f}, {0.0f, 0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        Vertex{{-1.0f,  1.0f, 1.0f}, {0.01f, 0.01f}, {0.0f, 0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        Vertex{{-1.0f, -1.0f, 1.0f}, {0.01f, 0.99f}, {0.0f, 0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},

        // top
        Vertex{{ 1.0f, 1.0f,  1.0f}, {0.99f, 0.01f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        Vertex{{ 1.0f, 1.0f, -1.0f}, {0.99f, 0.99f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        Vertex{{-1.0f, 1.0f, -1.0f}, {0.01f, 0.99f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},

        Vertex{{-1.0f, 1.0f, -1.0f}, {0.01f, 0.99f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        Vertex{{-1.0f, 1.0f,  1.0f}, {0.01f, 0.01f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        Vertex{{ 1.0f, 1.0f,  1.0f}, {0.99f, 0.01f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},

        // down
        Vertex{{ 1.0f, -1.0f,  1.0f}, {0.99f, 0.01f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        Vertex{{-1.0f, -1.0f, -1.0f}, {0.01f, 0.99f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        Vertex{{ 1.0f, -1.0f, -1.0f}, {0.99f, 0.99f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},

        Vertex{{-1.0f, -1.0f, -1.0f}, {0.01f, 0.99f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        Vertex{{ 1.0f, -1.0f,  1.0f}, {0.99f, 0.01f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        Vertex{{-1.0f, -1.0f,  1.0f}, {0.01f, 0.01f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
    };
    
    // CREATE INDEX BUFFER
    int indices[] =
    {
        0, 1, 2,
        3, 4, 5,
        6, 7, 8,
        9, 10, 11,
        12, 13, 14,
        15, 16, 17,
        18, 19, 20,
        21, 22, 23,
        24, 25, 26,
        27, 28, 29,
        30, 31, 32,
        33, 34, 35,
    };
    
    Model model(vertices, 36, indices, 36);
    auto result = models.try_emplace(key, std::make_shared<Model>(model));

    return result.first->second;
}

std::shared_ptr<Model> ModelManager::getDefaultPlane(const std::string & key)
{
    auto item = models.find(key);
    if (item != models.end()) return item->second;
    
    typedef Model::Vertex Vertex;

    // CREATE VERTEX BUFFER
    Vertex vertices[] =
    {
        // UV with a little offset to disable wrapping
        //     POSITION              UV            NORMAL               TANGENT             BITANGENT
        Vertex{{-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        Vertex{{-1.0f,  1.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        Vertex{{ 1.0f,  1.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        Vertex{{ 1.0f, -1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
    };
    
    // CREATE INDEX BUFFER
    int indices[] =
    {
        0, 1, 2,
        2, 3, 0,
    };
    
    Model model(vertices, 4, indices, 6);
    auto result = models.try_emplace(key, std::make_shared<Model>(model));

    return result.first->second;
}

std::shared_ptr<Model> ModelManager::getDefaultSphere(const std::string & key)
{
    auto item = models.find(key);
    if (item != models.end()) return item->second;
    
    typedef Model::Vertex Vertex;

    // GENERATE SPHERE FROM CUBE
    const uint32_t SIDES = 6;
    const uint32_t GRID_SIZE = 12;
    const uint32_t TRIS_PER_SIDE = GRID_SIZE * GRID_SIZE * 2;
    const uint32_t VERT_PER_SIDE = TRIS_PER_SIDE * 3;
    
    std::vector<Vertex> vertices;
    vertices.resize(VERT_PER_SIDE * SIDES);
    Vertex * vertex = vertices.data();
    
    int side_masks[6][3] =
    {
        {0, 1, 2}, // front
        {1, 0, 2}, // back
        {2, 1, 0}, // right
        {2, 1, 0}, // left
        {0, 2, 1}, // top
        {0, 2, 1}  // bot
    };

    float side_signs[6][3] =
    {
        {+1, +1, +1}, // front
        {+1, +1, -1}, // back
        {+1, +1, -1}, // right
        {-1, +1, +1}, // left
        {+1, +1, -1}, // top
        {+1, -1, +1}  // bot
    };

    for (int side = 0; side != SIDES; ++side)
    {
        for (int row = 0; row != GRID_SIZE; ++row)
        {
            for (int col = 0; col != GRID_SIZE; ++col)
            {
                float left = (col + 0) / float(GRID_SIZE) * 2.0f - 1.0f;
                float right = (col + 1) / float(GRID_SIZE) * 2.0f - 1.0f;
                float bottom = (row + 0) / float(GRID_SIZE) * 2.0f - 1.0f;
                float top = (row + 1) / float(GRID_SIZE) * 2.0f - 1.0f;

                // front quad (will be transformed to the other side
                // using side_masks and side_signs)
                glm::vec3 quad[4] =
                {
                    {left, bottom, -1.0f},
                    {right, bottom, -1.0f},
                    {left, top, -1.0f},
                    {right, top, -1.0f}
                };

                // (0, 0) - top left corner
                // V is inversed, because the quads starts from the left bot corner
                float uv_left = (col + 0) / float(GRID_SIZE);
                float uv_right = (col + 1) / float(GRID_SIZE);
                float uv_bottom = (GRID_SIZE - (row + 0)) / float(GRID_SIZE);
                float uv_top = (GRID_SIZE - (row + 1)) / float(GRID_SIZE);
                
                glm::vec2 uv[4]
                {
                    {uv_left, uv_bottom},
                    {uv_right, uv_bottom},
                    {uv_left, uv_top},
                    {uv_right, uv_top}
                };

                auto set_pos = [side_masks, side_signs](int side,
                                                        Vertex & dst,
                                                        const glm::vec3 & pos,
                                                        const glm::vec2 & uv)
                {
                    dst.position[side_masks[side][0]] = pos.x * side_signs[side][0];
                    dst.position[side_masks[side][1]] = pos.y * side_signs[side][1];
                    dst.position[side_masks[side][2]] = pos.z * side_signs[side][2];

                    dst.position = glm::normalize(dst.position);

                    dst.uv = uv;
                };

                set_pos(side, vertex[0], quad[0], uv[0]);
                set_pos(side, vertex[1], quad[2], uv[2]);
                set_pos(side, vertex[2], quad[1], uv[1]);

                {
                    glm::vec3 AB = vertex[1].position - vertex[0].position;
                    glm::vec3 AC = vertex[2].position - vertex[0].position;
                    vertex[0].normal = vertex[1].normal = vertex[2].normal =
                        glm::normalize(glm::cross(AB, AC));

                    vertex[0].tangent = vertex[1].tangent = vertex[2].tangent = AB;
                    vertex[0].bitangent = vertex[1].bitangent = vertex[2].bitangent = AC;
                }

                vertex += 3;

                set_pos(side, vertex[0], quad[1], uv[1]);
                set_pos(side, vertex[1], quad[2], uv[2]);
                set_pos(side, vertex[2], quad[3], uv[3]);

                {
                    glm::vec3 AB = vertex[1].position - vertex[0].position;
                    glm::vec3 AC = vertex[2].position - vertex[0].position;
                    vertex[0].normal = vertex[1].normal = vertex[2].normal =
                        glm::normalize(glm::cross(AB, AC));

                    vertex[0].tangent = vertex[1].tangent = vertex[2].tangent = AB;
                    vertex[0].bitangent = vertex[1].bitangent = vertex[2].bitangent = AC;
                }

                vertex += 3;
            }
        }
    }

    std::vector<int> indices;
    indices.resize(vertices.size());
    for (int i = 0, size = indices.size(); i != size; ++i) indices[i] = i;
    
    Model model(vertices.data(), vertices.size(),
                indices.data(), indices.size());
    auto result = models.try_emplace(key, std::make_shared<Model>(model));

    return result.first->second;
}
} // namespace engine
