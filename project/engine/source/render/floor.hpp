#ifndef FLOOR_HPP
#define FLOOR_HPP

#include <string>
#include <memory>

#include "globals.hpp"
#include "texture.hpp"
#include "shader.hpp"

namespace engine
{
class Floor
{
public:
    Floor() = default;

    void init(std::shared_ptr<Shader> shader,
              std::shared_ptr<Texture> texture);

    void render();

    std::shared_ptr<Shader> m_shader;
    std::shared_ptr<Texture> m_texture;
};
} // namespace engine

#endif
