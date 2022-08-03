#include "scene.hpp"

namespace engine
{
void Scene::renderFrame(windows::Window & window,
                        const Camera & camera)
{
    Globals * globals = Globals::getInstance();
    TextureManager * tex_mgr = TextureManager::getInstance();
    ShaderManager * shader_mgr = ShaderManager::getInstance();
    ModelManager * model_mgr = ModelManager::getInstance();
    MeshSystem * mesh_system = MeshSystem::getInstance();

    globals->bind(camera);

    window.bindRT();
    window.clearFrame();

    mesh_system->render();
    floor.render();
    sky.render();

    window.switchBuffer();
}
} // namespace engine

