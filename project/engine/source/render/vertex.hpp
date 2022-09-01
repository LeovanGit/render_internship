#ifndef VERTEX_HPP
#define VERTEX_HPP

#include "glm.hpp"

namespace engine
{
struct Vertex
{
    glm::vec3 position;
    glm::vec2 uv;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec3 bitangent;
};
} // namespace engine

#endif
