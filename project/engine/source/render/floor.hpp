#ifndef FLOOR_HPP
#define FLOOR_HPP

#include <string>

#include "globals.hpp"
#include "texture.hpp"
#include "shader.hpp"

namespace engine
{
class Floor
{
public:
    Floor() = default;

    void init(Shader & shader,
              Texture & texture);

    void render();

    Shader * m_shader;
    Texture * m_texture;
};
} // namespace engine

#endif
