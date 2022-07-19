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

    m_shader->activate();
    m_texture->activate();
    globals->device_context4->Draw(3, 0);
}
} // namespace engine
