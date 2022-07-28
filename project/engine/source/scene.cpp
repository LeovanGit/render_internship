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

    // which type of primitive to use (triangles)
    globals->device_context4->
        IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // ====> DRAW KNIGHT
    tex_mgr->useTexture("cube");
    shader_mgr->useShader("opaque");
    model_mgr->useModel("knight");

    auto & meshes = model_mgr->getModel("knight").get_meshes();
        
    for (int i = 0; i != meshes.size(); ++i)
    {
        // just for test:
        if (i == 0) tex_mgr->useTexture("fur");
        else if (i == 1) tex_mgr->useTexture("legs");
        else if (i == 2) tex_mgr->useTexture("torso");
        else if (i == 3) tex_mgr->useTexture("head");
        else if (i == 4) tex_mgr->useTexture("eye");
        else if (i == 5) tex_mgr->useTexture("helmet");
        else if (i == 6) tex_mgr->useTexture("skirt");
        else if (i == 7) tex_mgr->useTexture("cape");
        else if (i == 8) tex_mgr->useTexture("glove");

        globals->device_context4->DrawIndexed(meshes[i].index_count,
                                              meshes[i].index_offset,
                                              meshes[i].vertex_offset);
    }

    // ====> DRAW FLOOR
    shader_mgr->useShader("floor");
    tex_mgr->useTexture("floor");
    globals->device_context4->Draw(6, 0);

    // ====> DRAW SKY
    m_sky.render();

    window.switchBuffer();
}
} // namespace engine

