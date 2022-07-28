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

    // ====> DRAW KNIGHT
    // tex_mgr->useTexture("cube");
    shader_mgr->useShader("opaque");
    // model_mgr->useModel("knight");
    
    // which type of primitive to use (triangles)
    globals->device_context4->
        IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // draw VBO to the back buffer
    std::vector<Model::Mesh> & meshes = model_mgr->getModel("knight").get_meshes();

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
        
        VertexBuffer<Vertex> & vbo = meshes[i].vertex_buffer;
        IndexBuffer & ibo = meshes[i].index_buffer;
        
        globals->device_context4->IASetVertexBuffers(0,
                                                     1,
                                                     vbo.get_data().get(),
                                                     &vbo.get_stride(),
                                                     &vbo.get_offset());

        globals->device_context4->IASetIndexBuffer(ibo.get_data().ptr(),
                                                   DXGI_FORMAT_R32_UINT,
                                                   0);

        globals->device_context4->DrawIndexed(ibo.get_size(),
                                              0,
                                              0);
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

