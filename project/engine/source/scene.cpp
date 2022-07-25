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

    // ====> DRAW CUBE
    tex_mgr->useTexture("cube");
    shader_mgr->useShader("cube");
    model_mgr->useModel("cube");
    
    // which type of primitive to use (triangles)
    globals->device_context4->
        IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // draw VBO to the back buffer
    globals->device_context4->Draw(36, 0);

    // ====> DRAW SKY
    m_sky.render();

    // ====> DRAW FLOOR
    shader_mgr->useShader("floor");
    tex_mgr->useTexture("floor");
    globals->device_context4->Draw(6, 0);

    window.switchBuffer();
}
} // namespace engine

