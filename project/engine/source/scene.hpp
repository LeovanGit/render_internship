#ifndef SCENE_HPP
#define SCENE_HPP

#include "window.hpp"
#include "camera.hpp"
#include "globals.hpp"
#include "texture_manager.hpp"
#include "shader_manager.hpp"
#include "model_manager.hpp"
#include "sky.hpp"
#include "mesh_system.hpp"

namespace engine
{
class Scene
{
public:
    Scene() = default;

    void renderFrame(windows::Window & window,
                     const Camera & camera);

    Sky sky;
    MeshSystem mesh_system;
};
} // namespace engine

#endif
