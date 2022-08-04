#include "floor.hpp"

namespace engine
{
void Floor::init(std::shared_ptr<Shader> shader,
                 std::shared_ptr<Texture> texture)
{
    m_shader = shader;
    m_texture = texture;
}

void Floor::render()
{
    Globals * globals = Globals::getInstance();

    globals->device_context4->
        IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    
    m_shader->bind();
    m_texture->bind();
    globals->device_context4->Draw(6, 0);
}
} // namespace engine
