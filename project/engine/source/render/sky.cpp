#include "sky.hpp"

namespace engine
{
void Sky::init(Shader & shader,
               Texture & texture)
{
    m_shader = &shader;
    m_texture = &texture;
}

void Sky::render()
{
    Globals * globals = Globals::getInstance();

    m_shader->bind();
    m_texture->bind();
    globals->device_context4->Draw(3, 0);
}
} // namespace engine
