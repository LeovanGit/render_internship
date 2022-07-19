#ifndef SCENE_HPP
#define SCENE_HPP

#include "window.hpp"
#include "camera.hpp"
#include "sky.hpp"
#include "globals.hpp"
#include "texture_manager.hpp"
#include "shader_manager.hpp"

namespace engine
{
class Scene
{
public:
    Scene() = default;

    void renderFrame(windows::Window & window,
                     Camera & camera);

    Sky m_sky;
};
} // namespace engine

#endif
