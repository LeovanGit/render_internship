#ifndef SKY_HPP
#define SKY_HPP

#include <string>

#include "globals.hpp"
#include "texture.hpp"
#include "shader.hpp"

namespace engine
{
class Sky
{
public:
    Sky() = default;

    void init(Shader & shader,
              Texture & texture);

    void render();

    Shader * m_shader;
    Texture * m_texture;
};
} // namespace engine

#endif
