#include "scene.hpp"

namespace engine
{
void Scene::renderFrame(windows::Window & window,
                        const Camera & camera)
{
    HRESULT result;
    
    Globals * globals = Globals::getInstance();
    TextureManager * tex_mgr = TextureManager::getInstance();
    ShaderManager * shader_mgr = ShaderManager::getInstance();
    ModelManager * model_mgr = ModelManager::getInstance();

    globals->bind(camera);

    window.bindRT();
    window.clearFrame();

    // DRAW MODELS
    mesh_system.render();
    
    // DRAW FLOOR
    globals->device_context4->
        IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    shader_mgr->useShader("floor");
    tex_mgr->useTexture("floor");
    
    globals->device_context4->Draw(6, 0);

    // DRAW SKY
    sky.render();

    window.switchBuffer();
}
} // namespace engine

