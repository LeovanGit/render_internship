#include "controller.hpp"

void Controller::init(engine::Renderer & renderer,
                      engine::Postprocess & post_process)
{
    this->renderer = &renderer;
    this->post_process = &post_process;

    mouse = glm::vec2(0);
    fixed_mouse = glm::vec2(0);

    is_accelerated = false;

    for (int i = 0; i != KEYS_COUNT; ++i) keys_log[i] = false;
    for (int i = 0; i != KEYS_COUNT; ++i) was_released[i] = true;
}

void Controller::initScene(Camera & camera)
{
    camera.setPerspective(glm::radians(45.0f),
                          1280.0f / 720.0f,
                          1.0f,
                          1000.0f);

    initShaders();
    initTextures();
    initSceneObjects();
    initParticleEmitters();
    initGrassFields();
}

void Controller::initPostprocess()
{
    engine::ShaderManager * shader_mgr = engine::ShaderManager::getInstance();
    
    post_process->init(shader_mgr->getShader("../engine/shaders/resolve.hlsl"), 0.5f);
}

void Controller::initKnight(const math::Transform & transform)
{    
    engine::TextureManager * tex_mgr = engine::TextureManager::getInstance();
    engine::ModelManager * model_mgr = engine::ModelManager::getInstance();
    engine::MeshSystem * mesh_system = engine::MeshSystem::getInstance();
    engine::TransformSystem * trans_system = engine::TransformSystem::getInstance();

    std::vector<oi::Material> materials =
    {
        oi::Material(tex_mgr->getTexture("../engine/assets/Knight/dds/Fur_BaseColor.dds"),
                     tex_mgr->getTexture("../engine/assets/Knight/dds/Fur_Roughness.dds"),
                     nullptr,
                     tex_mgr->getTexture("../engine/assets/Knight/dds/Fur_Normal.dds"),
                     true,
                     true,
                     glm::vec3(1.0f, 0.0f, 0.0f),
                     0.95f,
                     0.0f),
        
        oi::Material(tex_mgr->getTexture("../engine/assets/Knight/dds/Legs_BaseColor.dds"),
                     tex_mgr->getTexture("../engine/assets/Knight/dds/Legs_Roughness.dds"),
                     tex_mgr->getTexture("../engine/assets/Knight/dds/Legs_Metallic.dds"),
                     tex_mgr->getTexture("../engine/assets/Knight/dds/Legs_Normal.dds")),

        oi::Material(tex_mgr->getTexture("../engine/assets/Knight/dds/Torso_BaseColor.dds"),
                     tex_mgr->getTexture("../engine/assets/Knight/dds/Torso_Roughness.dds"),
                     tex_mgr->getTexture("../engine/assets/Knight/dds/Torso_Metallic.dds"),
                     tex_mgr->getTexture("../engine/assets/Knight/dds/Torso_Normal.dds")),

        oi::Material(tex_mgr->getTexture("../engine/assets/Knight/dds/Head_BaseColor.dds"),
                     tex_mgr->getTexture("../engine/assets/Knight/dds/Head_Roughness.dds"),
                     nullptr,
                     tex_mgr->getTexture("../engine/assets/Knight/dds/Head_Normal.dds"),
                     true,
                     false,
                     glm::vec3(1.0f, 0.0f, 0.0f),
                     0.95f,
                     0.0f),
        
        oi::Material(tex_mgr->getTexture("../engine/assets/Knight/dds/Eye_BaseColor.dds"),
                     nullptr,
                     nullptr,
                     nullptr,
                     true,
                     false,
                     glm::vec3(1.0f, 0.0f, 0.0f),
                     0.1f,
                     0.0f),
        
        oi::Material(tex_mgr->getTexture("../engine/assets/Knight/dds/Helmet_BaseColor.dds"),
                     tex_mgr->getTexture("../engine/assets/Knight/dds/Helmet_Roughness.dds"),
                     tex_mgr->getTexture("../engine/assets/Knight/dds/Helmet_Metallic.dds"),
                     tex_mgr->getTexture("../engine/assets/Knight/dds/Helmet_Normal.dds"),
                     true,
                     true),
        
        oi::Material(tex_mgr->getTexture("../engine/assets/Knight/dds/Skirt_BaseColor.dds"),
                     tex_mgr->getTexture("../engine/assets/Knight/dds/Skirt_Roughness.dds"),
                     nullptr,
                     tex_mgr->getTexture("../engine/assets/Knight/dds/Skirt_Normal.dds"),
                     true,
                     true,
                     glm::vec3(1.0f, 0.0f, 0.0f),
                     0.95f,
                     0.0f),
        
        oi::Material(tex_mgr->getTexture("../engine/assets/Knight/dds/Cape_BaseColor.dds"),
                     tex_mgr->getTexture("../engine/assets/Knight/dds/Cape_Roughness.dds"),
                     nullptr,
                     tex_mgr->getTexture("../engine/assets/Knight/dds/Cape_Normal.dds"),
                     true,
                     true,
                     glm::vec3(1.0f, 0.0f, 0.0f),
                     0.95f,
                     0.0f),
        
        oi::Material(tex_mgr->getTexture("../engine/assets/Knight/dds/Glove_BaseColor.dds"),
                     tex_mgr->getTexture("../engine/assets/Knight/dds/Glove_Roughness.dds"),
                     tex_mgr->getTexture("../engine/assets/Knight/dds/Glove_Metallic.dds"),
                     tex_mgr->getTexture("../engine/assets/Knight/dds/Glove_Normal.dds")),
    };

    uint32_t transform_id = trans_system->transforms.insert(transform);

    mesh_system->addInstance<engine::OpaqueInstances>(
        model_mgr->getModel("../engine/assets/Knight/Knight.fbx"),
        materials,
        transform_id);
}

void Controller::spawnKnight(const math::Transform & transform)
{    
    engine::TextureManager * tex_mgr = engine::TextureManager::getInstance();
    engine::ModelManager * model_mgr = engine::ModelManager::getInstance();
    engine::MeshSystem * mesh_system = engine::MeshSystem::getInstance();
    engine::TransformSystem * trans_system = engine::TransformSystem::getInstance();

    std::vector<di::Material> materials =
    {
        di::Material(tex_mgr->getTexture("../engine/assets/Knight/dds/Fur_BaseColor.dds"),
                     tex_mgr->getTexture("../engine/assets/Knight/dds/Fur_Roughness.dds"),
                     nullptr,
                     tex_mgr->getTexture("../engine/assets/Knight/dds/Fur_Normal.dds"),
                     true,
                     true,
                     glm::vec3(1.0f, 0.0f, 0.0f),
                     0.95f,
                     0.0f),
        
        di::Material(tex_mgr->getTexture("../engine/assets/Knight/dds/Legs_BaseColor.dds"),
                     tex_mgr->getTexture("../engine/assets/Knight/dds/Legs_Roughness.dds"),
                     tex_mgr->getTexture("../engine/assets/Knight/dds/Legs_Metallic.dds"),
                     tex_mgr->getTexture("../engine/assets/Knight/dds/Legs_Normal.dds")),

        di::Material(tex_mgr->getTexture("../engine/assets/Knight/dds/Torso_BaseColor.dds"),
                     tex_mgr->getTexture("../engine/assets/Knight/dds/Torso_Roughness.dds"),
                     tex_mgr->getTexture("../engine/assets/Knight/dds/Torso_Metallic.dds"),
                     tex_mgr->getTexture("../engine/assets/Knight/dds/Torso_Normal.dds")),

        di::Material(tex_mgr->getTexture("../engine/assets/Knight/dds/Head_BaseColor.dds"),
                     tex_mgr->getTexture("../engine/assets/Knight/dds/Head_Roughness.dds"),
                     nullptr,
                     tex_mgr->getTexture("../engine/assets/Knight/dds/Head_Normal.dds"),
                     true,
                     false,
                     glm::vec3(1.0f, 0.0f, 0.0f),
                     0.95f,
                     0.0f),
        
        di::Material(tex_mgr->getTexture("../engine/assets/Knight/dds/Eye_BaseColor.dds"),
                     nullptr,
                     nullptr,
                     nullptr,
                     true,
                     false,
                     glm::vec3(1.0f, 0.0f, 0.0f),
                     0.1f,
                     0.0f),
        
        di::Material(tex_mgr->getTexture("../engine/assets/Knight/dds/Helmet_BaseColor.dds"),
                     tex_mgr->getTexture("../engine/assets/Knight/dds/Helmet_Roughness.dds"),
                     tex_mgr->getTexture("../engine/assets/Knight/dds/Helmet_Metallic.dds"),
                     tex_mgr->getTexture("../engine/assets/Knight/dds/Helmet_Normal.dds"),
                     true,
                     true),
        
        di::Material(tex_mgr->getTexture("../engine/assets/Knight/dds/Skirt_BaseColor.dds"),
                     tex_mgr->getTexture("../engine/assets/Knight/dds/Skirt_Roughness.dds"),
                     nullptr,
                     tex_mgr->getTexture("../engine/assets/Knight/dds/Skirt_Normal.dds"),
                     true,
                     true,
                     glm::vec3(1.0f, 0.0f, 0.0f),
                     0.95f,
                     0.0f),
        
        di::Material(tex_mgr->getTexture("../engine/assets/Knight/dds/Cape_BaseColor.dds"),
                     tex_mgr->getTexture("../engine/assets/Knight/dds/Cape_Roughness.dds"),
                     nullptr,
                     tex_mgr->getTexture("../engine/assets/Knight/dds/Cape_Normal.dds"),
                     true,
                     true,
                     glm::vec3(1.0f, 0.0f, 0.0f),
                     0.95f,
                     0.0f),
        
        di::Material(tex_mgr->getTexture("../engine/assets/Knight/dds/Glove_BaseColor.dds"),
                     tex_mgr->getTexture("../engine/assets/Knight/dds/Glove_Roughness.dds"),
                     tex_mgr->getTexture("../engine/assets/Knight/dds/Glove_Metallic.dds"),
                     tex_mgr->getTexture("../engine/assets/Knight/dds/Glove_Normal.dds")),
    };

    uint32_t transform_id = trans_system->transforms.insert(transform);
    float spawn_time = engine::TimeSystem::getTimePoint();

    di::Instance instance(transform_id,
                          spawn_time,
                          3.0f);
    
    mesh_system->addInstance<engine::DissolutionInstances>(
        model_mgr->getModel("../engine/assets/Knight/Knight.fbx"),
        materials,
        instance);
}

void Controller::initWall(const math::Transform & transform)
{
    engine::TextureManager * tex_mgr = engine::TextureManager::getInstance();
    engine::ModelManager * model_mgr = engine::ModelManager::getInstance();
    engine::MeshSystem * mesh_system = engine::MeshSystem::getInstance();
    engine::TransformSystem * trans_system = engine::TransformSystem::getInstance();

    std::vector<oi::Material> materials =
    {
        oi::Material(tex_mgr->getTexture("../engine/assets/Wall/dds/Star_BaseColor.dds"),
                     tex_mgr->getTexture("../engine/assets/Wall/dds/Star_Roughness.dds"),
                     tex_mgr->getTexture("../engine/assets/Wall/dds/Star_Metallic.dds"),
                     tex_mgr->getTexture("../engine/assets/Wall/dds/Star_Normal.dds")),

        oi::Material(tex_mgr->getTexture("../engine/assets/Wall/dds/Marble_BaseColor.dds"),
                     nullptr,
                     nullptr,
                     nullptr,
                     true,
                     false,
                     glm::vec3(1.0f, 0.0f, 0.0f),
                     0.95f,
                     0.0f),
        
        oi::Material(tex_mgr->getTexture("../engine/assets/Wall/dds/StoneWork_BaseColor.dds"),
                     tex_mgr->getTexture("../engine/assets/Wall/dds/StoneWork_Roughness.dds"),
                     nullptr,
                     tex_mgr->getTexture("../engine/assets/Wall/dds/StoneWork_Normal.dds"),
                     true,
                     false,                     
                     glm::vec3(1.0f, 0.0f, 0.0f),
                     0.99f,
                     0.0f),

        oi::Material(tex_mgr->getTexture("../engine/assets/Wall/dds/Statue_BaseColor.dds"),
                     tex_mgr->getTexture("../engine/assets/Wall/dds/Statue_Roughness.dds"),
                     nullptr,
                     tex_mgr->getTexture("../engine/assets/Wall/dds/Statue_Normal.dds"),
                     true,
                     false,
                     glm::vec3(1.0f, 0.0f, 0.0f),
                     0.01f,
                     1.0f),
        
        oi::Material(tex_mgr->getTexture("../engine/assets/Wall/dds/Trims_BaseColor.dds"),
                     tex_mgr->getTexture("../engine/assets/Wall/dds/Trims_Roughness.dds"),
                     tex_mgr->getTexture("../engine/assets/Wall/dds/Trims_Metallic.dds"),
                     tex_mgr->getTexture("../engine/assets/Wall/dds/Trims_Normal.dds")),
    };

    uint32_t transform_id = trans_system->transforms.insert(transform);
    
    mesh_system->addInstance<engine::OpaqueInstances>(
        model_mgr->getModel("../engine/assets/Wall/SunCityWall.fbx"),
        materials,
        transform_id);
}

void Controller::initCube(const math::Transform & transform,
                          const std::vector<oi::Material> & materials)
{
    engine::ModelManager * model_mgr = engine::ModelManager::getInstance();
    engine::MeshSystem * mesh_system = engine::MeshSystem::getInstance();
    engine::TransformSystem * trans_system = engine::TransformSystem::getInstance();

    uint32_t transform_id = trans_system->transforms.insert(transform);

    mesh_system->addInstance<engine::OpaqueInstances>(model_mgr->getDefaultCube("cube"),
                                                      materials,
                                                      transform_id);
}

void Controller::initPlane(const math::Transform & transform,
                           const std::vector<oi::Material> & materials)
{
    engine::ModelManager * model_mgr = engine::ModelManager::getInstance();
    engine::MeshSystem * mesh_system = engine::MeshSystem::getInstance();
    engine::TransformSystem * trans_system = engine::TransformSystem::getInstance();

    uint32_t transform_id = trans_system->transforms.insert(transform);

    mesh_system->addInstance<engine::OpaqueInstances>(model_mgr->getDefaultPlane("plane"),
                                                      materials,
                                                      transform_id);
}

void Controller::initSphere(const math::Transform & transform,
                            const std::vector<oi::Material> & materials)
{
    engine::ModelManager * model_mgr = engine::ModelManager::getInstance();
    engine::MeshSystem * mesh_system = engine::MeshSystem::getInstance();
    engine::TransformSystem * trans_system = engine::TransformSystem::getInstance();

    uint32_t transform_id = trans_system->transforms.insert(transform);

    mesh_system->addInstance<engine::OpaqueInstances>(model_mgr->getDefaultSphere("sphere"),
                                                      materials,
                                                      transform_id);
}

void Controller::initFloor(const std::vector<oi::Material> & materials)
{
    engine::ModelManager * model_mgr = engine::ModelManager::getInstance();
    engine::MeshSystem * mesh_system = engine::MeshSystem::getInstance();
    
    initPlane(math::Transform(glm::vec3(25.0f, -10.0f, -25.0f),
                              math::EulerAngles(0.0f, 90.0f, 0.0f),
                              glm::vec3(25.0f)),
              materials);
              
    initPlane(math::Transform(glm::vec3(-25.0f, -10.0f, -25.0f),
                              math::EulerAngles(0.0f, 90.0f, 0.0f),
                              glm::vec3(25.0f)),
              materials);
    
    initPlane(math::Transform(glm::vec3(25.0f, -10.0f, 25.0f),
                              math::EulerAngles(0.0f, 90.0f, 0.0f),
                              glm::vec3(25.0f)),
              materials);
    
    initPlane(math::Transform(glm::vec3(-25.0f, -10.0f, 25.0f),
                              math::EulerAngles(0.0f, 90.0f, 0.0f),
                              glm::vec3(25.0f)),
              materials);
}

void Controller::initSwamp(const std::vector<oi::Material> & materials)
{
    engine::ModelManager * model_mgr = engine::ModelManager::getInstance();
    engine::MeshSystem * mesh_system = engine::MeshSystem::getInstance();
    
    initPlane(math::Transform(glm::vec3(125.0f, -10.0f, -25.0f),
                              math::EulerAngles(0.0f, 90.0f, 0.0f),
                              glm::vec3(25.0f)),
              materials);
              
    initPlane(math::Transform(glm::vec3(75.0f, -10.0f, -25.0f),
                              math::EulerAngles(0.0f, 90.0f, 0.0f),
                              glm::vec3(25.0f)),
              materials);
    
    initPlane(math::Transform(glm::vec3(125.0f, -10.0f, 25.0f),
                              math::EulerAngles(0.0f, 90.0f, 0.0f),
                              glm::vec3(25.0f)),
              materials);
    
    initPlane(math::Transform(glm::vec3(75.0f, -10.0f, 25.0f),
                              math::EulerAngles(0.0f, 90.0f, 0.0f),
                              glm::vec3(25.0f)),
              materials);    
}

void Controller::initDirectionalLight(const glm::vec3 & radiance,
                                      const glm::vec3 & direction,
                                      float solid_angle)
{
    engine::LightSystem * light_system = engine::LightSystem::getInstance();

    light_system->addDirectionalLight(radiance, direction, solid_angle);
}

void Controller::initPointLight(const glm::vec3 & position,
                                const glm::vec3 & irradiance,
                                float distance,
                                float radius)
{
    engine::LightSystem * light_system = engine::LightSystem::getInstance();
    engine::MeshSystem * mesh_system = engine::MeshSystem::getInstance();
    engine::ModelManager * model_mgr = engine::ModelManager::getInstance();
    engine::TransformSystem * trans_system = engine::TransformSystem::getInstance();

    glm::vec3 radiance = light_system->radianceFromIrradianceAtDistance(irradiance,
                                                                        distance,
                                                                        radius);

    math::Transform transform(position,
                              math::EulerAngles(0.0f, 0.0f, 0.0f),
                              glm::vec3(radius));
    
    uint32_t transform_id = trans_system->transforms.insert(transform);
    
    // data
    light_system->addPointLight(transform_id, radiance, radius);

    // visualization
    std::vector<ei::Material> materials =
    {
        ei::Material(),
    };
    
    mesh_system->addInstance<engine::EmissiveInstances>(model_mgr->getDefaultSphere("sphere"),
                                                        materials,
                                                        ei::Instance(transform_id, radiance));
}

void Controller::initShaders()
{
    engine::ShaderManager * shader_mgr = engine::ShaderManager::getInstance();
    engine::MeshSystem * mesh_system = engine::MeshSystem::getInstance();
    engine::ParticleSystem * particle_sys = engine::ParticleSystem::getInstance();
    engine::GrassSystem * grass_sys = engine::GrassSystem::getInstance();
    
    D3D11_INPUT_ELEMENT_DESC ied_opaque[] =
    {
        {"POSITION",
         0,
         DXGI_FORMAT_R32G32B32_FLOAT,
         0,
         0,
         D3D11_INPUT_PER_VERTEX_DATA,
         0},

        {"TEXCOORD",
         0,
         DXGI_FORMAT_R32G32_FLOAT,
         0,
         12, // 3 floats of 4 bytes
         D3D11_INPUT_PER_VERTEX_DATA,
         0},

        {"NORMAL",
         0,
         DXGI_FORMAT_R32G32B32_FLOAT,
         0,
         20,
         D3D11_INPUT_PER_VERTEX_DATA,
         0},

        {"TANGENT",
         0,
         DXGI_FORMAT_R32G32B32_FLOAT,
         0,
         32,
         D3D11_INPUT_PER_VERTEX_DATA,
         0},

        {"BITANGENT",
         0,
         DXGI_FORMAT_R32G32B32_FLOAT,
         0,
         44,
         D3D11_INPUT_PER_VERTEX_DATA,
         0},
        
        {"TRANSFORM",
         0,
         DXGI_FORMAT_R32G32B32A32_FLOAT,
         1,
         0, // reset align for instance data!
         D3D11_INPUT_PER_INSTANCE_DATA,
         1},

        {"TRANSFORM",
         1,
         DXGI_FORMAT_R32G32B32A32_FLOAT,
         1,
         16,
         D3D11_INPUT_PER_INSTANCE_DATA,
         1},

        {"TRANSFORM",
         2,
         DXGI_FORMAT_R32G32B32A32_FLOAT,
         1,
         32,
         D3D11_INPUT_PER_INSTANCE_DATA,
         1},

        {"TRANSFORM",
         3,
         DXGI_FORMAT_R32G32B32A32_FLOAT,
         1,
         48,
         D3D11_INPUT_PER_INSTANCE_DATA,
         1},
    };
    
    D3D11_INPUT_ELEMENT_DESC ied_emissive[] =
    {
        {"POSITION",
         0,
         DXGI_FORMAT_R32G32B32_FLOAT,
         0,
         0,
         D3D11_INPUT_PER_VERTEX_DATA,
         0},

        {"TEXCOORD",
         0,
         DXGI_FORMAT_R32G32_FLOAT,
         0,
         12,
         D3D11_INPUT_PER_VERTEX_DATA,
         0},
        
        {"NORMAL",
         0,
         DXGI_FORMAT_R32G32B32_FLOAT,
         0,
         20,
         D3D11_INPUT_PER_VERTEX_DATA,
         0},

        {"TRANSFORM",
         0,
         DXGI_FORMAT_R32G32B32A32_FLOAT,
         1,
         0, // reset align for instance data!
         D3D11_INPUT_PER_INSTANCE_DATA,
         1},

        {"TRANSFORM",
         1,
         DXGI_FORMAT_R32G32B32A32_FLOAT,
         1,
         16,
         D3D11_INPUT_PER_INSTANCE_DATA,
         1},

        {"TRANSFORM",
         2,
         DXGI_FORMAT_R32G32B32A32_FLOAT,
         1,
         32,
         D3D11_INPUT_PER_INSTANCE_DATA,
         1},

        {"TRANSFORM",
         3,
         DXGI_FORMAT_R32G32B32A32_FLOAT,
         1,
         48,
         D3D11_INPUT_PER_INSTANCE_DATA,
         1},

        {"RADIANCE",
         0,
         DXGI_FORMAT_R32G32B32A32_FLOAT,
         1,
         64,
         D3D11_INPUT_PER_INSTANCE_DATA,
         1},
    };

    D3D11_INPUT_ELEMENT_DESC ied_shadow_map[] =
    {
        {"POSITION",
         0,
         DXGI_FORMAT_R32G32B32_FLOAT,
         0,
         0,
         D3D11_INPUT_PER_VERTEX_DATA,
         0},

        {"TRANSFORM",
         0,
         DXGI_FORMAT_R32G32B32A32_FLOAT,
         1,
         0, // reset align for instance data!
         D3D11_INPUT_PER_INSTANCE_DATA,
         1},

        {"TRANSFORM",
         1,
         DXGI_FORMAT_R32G32B32A32_FLOAT,
         1,
         16,
         D3D11_INPUT_PER_INSTANCE_DATA,
         1},

        {"TRANSFORM",
         2,
         DXGI_FORMAT_R32G32B32A32_FLOAT,
         1,
         32,
         D3D11_INPUT_PER_INSTANCE_DATA,
         1},

        {"TRANSFORM",
         3,
         DXGI_FORMAT_R32G32B32A32_FLOAT,
         1,
         48,
         D3D11_INPUT_PER_INSTANCE_DATA,
         1},
    };

    D3D11_INPUT_ELEMENT_DESC ied_dissolve[] =
    {
        {"POSITION",
         0,
         DXGI_FORMAT_R32G32B32_FLOAT,
         0,
         0,
         D3D11_INPUT_PER_VERTEX_DATA,
         0},

        {"TEXCOORD",
         0,
         DXGI_FORMAT_R32G32_FLOAT,
         0,
         12, // 3 floats of 4 bytes
         D3D11_INPUT_PER_VERTEX_DATA,
         0},

        {"NORMAL",
         0,
         DXGI_FORMAT_R32G32B32_FLOAT,
         0,
         20,
         D3D11_INPUT_PER_VERTEX_DATA,
         0},

        {"TANGENT",
         0,
         DXGI_FORMAT_R32G32B32_FLOAT,
         0,
         32,
         D3D11_INPUT_PER_VERTEX_DATA,
         0},

        {"BITANGENT",
         0,
         DXGI_FORMAT_R32G32B32_FLOAT,
         0,
         44,
         D3D11_INPUT_PER_VERTEX_DATA,
         0},
        
        {"TRANSFORM",
         0,
         DXGI_FORMAT_R32G32B32A32_FLOAT,
         1,
         0, // reset align for instance data!
         D3D11_INPUT_PER_INSTANCE_DATA,
         1},

        {"TRANSFORM",
         1,
         DXGI_FORMAT_R32G32B32A32_FLOAT,
         1,
         16,
         D3D11_INPUT_PER_INSTANCE_DATA,
         1},

        {"TRANSFORM",
         2,
         DXGI_FORMAT_R32G32B32A32_FLOAT,
         1,
         32,
         D3D11_INPUT_PER_INSTANCE_DATA,
         1},

        {"TRANSFORM",
         3,
         DXGI_FORMAT_R32G32B32A32_FLOAT,
         1,
         48,
         D3D11_INPUT_PER_INSTANCE_DATA,
         1},

        {"SPAWN_TIME",
         0,
         DXGI_FORMAT_R32_FLOAT,
         1,
         64,
         D3D11_INPUT_PER_INSTANCE_DATA,
         1},

        {"ANIMATION_TIME",
         0,
         DXGI_FORMAT_R32_FLOAT,
         1,
         68,
         D3D11_INPUT_PER_INSTANCE_DATA,
         1},
    };
    
    D3D11_INPUT_ELEMENT_DESC ied_particles[] =
    {
        {"POSITION",
         0,
         DXGI_FORMAT_R32G32B32_FLOAT,
         1,
         0,
         D3D11_INPUT_PER_INSTANCE_DATA,
         1},

        {"SIZE",
         0,
         DXGI_FORMAT_R32G32B32_FLOAT,
         1,
         12,
         D3D11_INPUT_PER_INSTANCE_DATA,
         1},

        {"ANGLE",
         0,
         DXGI_FORMAT_R32_FLOAT,
         1,
         24,
         D3D11_INPUT_PER_INSTANCE_DATA,
         1},
             
        {"TINT",
         0,
         DXGI_FORMAT_R32G32B32A32_FLOAT,
         1,
         28,
         D3D11_INPUT_PER_INSTANCE_DATA,
         1},

        {"LIFETIME",
         0,
         DXGI_FORMAT_R32_FLOAT,
         1,
         44,
         D3D11_INPUT_PER_INSTANCE_DATA,
         1},
    };
    
    D3D11_INPUT_ELEMENT_DESC ied_grass[] =
    {
        {"POSITION",
         0,
         DXGI_FORMAT_R32G32B32_FLOAT,
         1,
         0,
         D3D11_INPUT_PER_INSTANCE_DATA,
         1},

        {"SIZE",
         0,
         DXGI_FORMAT_R32G32_FLOAT,
         1,
         12,
         D3D11_INPUT_PER_INSTANCE_DATA,
         1},
    };

    D3D11_INPUT_ELEMENT_DESC ied_grass_shadows[] =
    {
        {"POSITION",
         0,
         DXGI_FORMAT_R32G32B32_FLOAT,
         1,
         0,
         D3D11_INPUT_PER_INSTANCE_DATA,
         1},

        {"SIZE",
         0,
         DXGI_FORMAT_R32G32_FLOAT,
         1,
         12,
         D3D11_INPUT_PER_INSTANCE_DATA,
         1},
    };
    
    mesh_system->setShaders(shader_mgr->getShader("../engine/shaders/opaque.hlsl",
                                                  ied_opaque,
                                                  9),
                            shader_mgr->getShader("../engine/shaders/emissive.hlsl",
                                                  ied_emissive,
                                                  8),
                            shader_mgr->getShader("../engine/shaders/shadow_cubemap.hlsl",
                                                  ied_shadow_map,
                                                  5,
                                                  true,
                                                  true,
                                                  false),
                            shader_mgr->getShader("../engine/shaders/dissolve.hlsl",
                                                  ied_dissolve,
                                                  11));

    particle_sys->shader =
        shader_mgr->getShader("../engine/shaders/particles.hlsl",
                              ied_particles,
                              5);

    grass_sys->shader =
        shader_mgr->getShader("../engine/shaders/grass.hlsl",
                              ied_grass,
                              2);

    grass_sys->shadow_shader =
        shader_mgr->getShader("../engine/shaders/grass_shadow_cubemap.hlsl",
                              ied_grass_shadows,
                              2,
                              true,
                              true,
                              true);

    renderer->deferred_shader =
        shader_mgr->getShader("../engine/shaders/deferred.hlsl");
}

void Controller::initTextures()
{
    engine::MeshSystem * mesh_system = engine::MeshSystem::getInstance();
    engine::TextureManager * tex_mgr = engine::TextureManager::getInstance();
    
    mesh_system->setTextures(tex_mgr->getTexture("../engine/assets/dissolve.dds"));
    renderer->reflectance = tex_mgr->getTexture("../engine/assets/environment/reflectance.dds");
    renderer->irradiance = tex_mgr->getTexture("../engine/assets/environment/irradiance.dds");
    renderer->reflection = tex_mgr->getTexture("../engine/assets/environment/reflection.dds");
}

void Controller::initSceneObjects()
{
    engine::ShaderManager * shader_mgr = engine::ShaderManager::getInstance();
    engine::TextureManager * tex_mgr = engine::TextureManager::getInstance();

    renderer->sky.init(shader_mgr->getShader("../engine/shaders/skybox.hlsl"),
                       tex_mgr->getTexture("../engine/assets/environment/skybox.dds"));

    initDirectionalLight(glm::normalize(glm::vec3(0.25f, -1.0f, 0.25f)),
                         glm::vec3(0.0f),
                         0.00006794f);

    initPointLight(glm::vec3(0.0f, 12.0f, 0.0f),
                   glm::vec3(1000.0f),
                   1.0f,
                   2.0f);

    initPointLight(glm::vec3(5.0f, 14.0f, 0.0f),
                   glm::vec3(1000.0f, 0.0f, 0.0f),
                   1.0f,
                   1.0f);

    initPointLight(glm::vec3(-5.0f, 10.0f, 0.0f),
                   glm::vec3(0.0f, 1000.0f, 0.0f),
                   1.0f,
                   1.0f);

    initPointLight(glm::vec3(-3.0f, 12.0f, 5.0f),
                   glm::vec3(0.0f, 0.0f, 1000.0f),
                   1.0f,
                   1.0f);

    initFloor(std::vector<oi::Material>{oi::Material(
                  tex_mgr->getTexture("../engine/assets/floor/tile_albedo.dds"),
                  tex_mgr->getTexture("../engine/assets/floor/tile_roughness.dds"),
                  tex_mgr->getTexture("../engine/assets/floor/tile_metallic.dds"),
                  tex_mgr->getTexture("../engine/assets/floor/tile_normal.dds"),
                  true,
                  true)});

    initSwamp(std::vector<oi::Material>{oi::Material(
                tex_mgr->getTexture("../engine/assets/swamp/albedo.dds"),
                tex_mgr->getTexture("../engine/assets/swamp/roughness.dds"),
                tex_mgr->getTexture("../engine/assets/swamp/metallic.dds"),
                tex_mgr->getTexture("../engine/assets/swamp/normal.dds"),
                true,
                true)});

    
    initSphere(math::Transform(glm::vec3(-13.0f, -6.0f, 42.0f),
                               math::EulerAngles(0.0f, 0.0f, 0.0f),
                               glm::vec3(4.0f, 4.0f, 4.0f)),
               std::vector<oi::Material>{oi::Material(
                 tex_mgr->getTexture("../engine/assets/rusted_iron/rusted_iron_albedo.dds"),
                 tex_mgr->getTexture("../engine/assets/rusted_iron/rusted_iron_roughness.dds"),
                 tex_mgr->getTexture("../engine/assets/rusted_iron/rusted_iron_metallic.dds"),
                 tex_mgr->getTexture("../engine/assets/rusted_iron/rusted_iron_normal.dds"))});
    
    initCube(math::Transform(glm::vec3(-30.0f, 0.0f, 0.0f),
                             math::EulerAngles(45.0f, 0.0f, 0.0f),
                             glm::vec3(4.0f, 4.0f, 4.0f)),
             std::vector<oi::Material>{oi::Material(
                 tex_mgr->getTexture("../engine/assets/space/space_albedo.dds"),
                 tex_mgr->getTexture("../engine/assets/space/space_roughness.dds"),
                 tex_mgr->getTexture("../engine/assets/space/space_metallic.dds"),
                 tex_mgr->getTexture("../engine/assets/space/space_normal.dds"))});

    initCube(math::Transform(glm::vec3(-30.0f, 0.0f, 20.0f),
                             math::EulerAngles(45.0f, 0.0f, 0.0f),
                             glm::vec3(4.0f, 4.0f, 4.0f)),
             std::vector<oi::Material>{oi::Material(
                 tex_mgr->getTexture("../engine/assets/rusted_iron/rusted_iron_albedo.dds"),
                 tex_mgr->getTexture("../engine/assets/rusted_iron/rusted_iron_roughness.dds"),
                 tex_mgr->getTexture("../engine/assets/rusted_iron/rusted_iron_metallic.dds"),
                 tex_mgr->getTexture("../engine/assets/rusted_iron/rusted_iron_normal.dds"))});

    initCube(math::Transform(glm::vec3(-30.0f, 0.0f, -20.0f),
                             math::EulerAngles(45.0f, 0.0f, 0.0f),
                             glm::vec3(4.0f, 4.0f, 4.0f)),
             std::vector<oi::Material>{oi::Material(
                 tex_mgr->getTexture("../engine/assets/leather/leather_albedo.dds"),
                 tex_mgr->getTexture("../engine/assets/leather/leather_roughness.dds"),
                 tex_mgr->getTexture("../engine/assets/leather/leather_metallic.dds"),
                 tex_mgr->getTexture("../engine/assets/leather/leather_normal.dds"))});
    
    initKnight(math::Transform(glm::vec3(15.0f, -10.0f, 0.0f),
                               math::EulerAngles(90.0f, 0.0f, 0.0f),
                               glm::vec3(10.0f, 10.0f, 10.0f)));

    initKnight(math::Transform(glm::vec3(-15.0f, -10.0f, 0.0f),
                               math::EulerAngles(-90.0f, 0.0f, 0.0f),
                               glm::vec3(10.0f, 10.0f, 10.0f)));

    initKnight(math::Transform(glm::vec3(0.0f, -10.0f, 15.0f),
                               math::EulerAngles(0.0f, 0.0f, 0.0f),
                               glm::vec3(10.0f, 10.0f, 10.0f)));

    initWall(math::Transform(glm::vec3(0.0f, -10.0f, 50.0f),
                             math::EulerAngles(0.0f, 0.0f, 0.0f),
                             glm::vec3(10.0f, 10.0f, 10.0f)));

    // gold mirror
    initPlane(math::Transform(glm::vec3(25.0f, 0.0f, 48.0f),
                              math::EulerAngles(0.0f, 0.0f, 0.0f),
                              glm::vec3(5.0f, 10.0f, 1.0f)),
              std::vector<oi::Material>{oi::Material(
                  glm::vec3(0.944f, 0.776f, 0.373f),
                  0.01f,
                  1.0f,
                  true,
                  true)});

    // copper mirror
    initPlane(math::Transform(glm::vec3(35.0f, 0.0f, 48.0f),
                              math::EulerAngles(0.0f, 0.0f, 0.0f),
                              glm::vec3(5.0f, 10.0f, 1.0f)),
              std::vector<oi::Material>{oi::Material(
                  glm::vec3(0.926f, 0.721f, 0.504f),
                  0.15f,
                  1.0f,
                  true,
                  true)});

    // iron mirror
    initPlane(math::Transform(glm::vec3(45.0f, 0.0f, 48.0f),
                              math::EulerAngles(0.0f, 0.0f, 0.0f),
                              glm::vec3(5.0f, 10.0f, 1.0f)),
              std::vector<oi::Material>{oi::Material(
                  glm::vec3(0.531f, 0.512f, 0.496f),
                  0.27f,
                  1.0f,
                  true,
                  true)});

    // red mirror
    initPlane(math::Transform(glm::vec3(55.0f, 0.0f, 48.0f),
                              math::EulerAngles(0.0f, 0.0f, 0.0f),
                              glm::vec3(5.0f, 10.0f, 1.0f)),
              std::vector<oi::Material>{oi::Material(
                  glm::vec3(1.0f, 0.3f, 0.3f),
                  0.4f,
                  1.0f,
                  true,
                  true)});
}

void Controller::initParticleEmitters()
{
    engine::ParticleSystem * particle_sys = engine::ParticleSystem::getInstance();
    engine::TextureManager * texture_mgr = engine::TextureManager::getInstance();

    particle_sys->lightmap_RLT = texture_mgr->
        getTexture("../engine/assets/smoke/lightmap_RLT.dds");

    particle_sys->lightmap_BotBF = texture_mgr->
        getTexture("../engine/assets/smoke/lightmap_BotBF.dds");
    
    particle_sys->motion_vectors = texture_mgr->
        getTexture("../engine/assets/smoke/motion_vectors.dds");
    
    // red
    particle_sys->addSmokeEmitter(
        engine::SmokeEmitter(glm::vec3(120.0f, -13.0f, 30.0f),
                             0.5f,
                             glm::vec3(1.0f, 0.0f, 0.0f),
                             0.1f,
                             5.0f,
                             0.5f,
                             0.05f,
                             0.25f));

    // white
    particle_sys->addSmokeEmitter(
        engine::SmokeEmitter(glm::vec3(120.0f, -12.0f, 0.0f),
                             3.0f,
                             glm::vec3(1.0f, 1.0f, 1.0f),
                             0.25f,
                             2.0f,
                             1.0f,
                             0.025f,
                             0.5f));

    // yellow
    particle_sys->addSmokeEmitter(
        engine::SmokeEmitter(glm::vec3(120.0f, -7.5f, -30.0f),
                             10.0f,
                             glm::vec3(0.86f, 0.47f, 0.0f),
                             0.25f,
                             0.2f,
                             1.0f,
                             0.025f,
                             0.1f));
}

void Controller::initGrassFields()
{
    engine::GrassSystem * grass_sys = engine::GrassSystem::getInstance();
    engine::TextureManager * texture_mgr = engine::TextureManager::getInstance();

    grass_sys->albedo = texture_mgr->
        getTexture("../engine/assets/grass/albedo.dds");

    grass_sys->opacity = texture_mgr->
        getTexture("../engine/assets/grass/opacity.dds");

    grass_sys->roughness = texture_mgr->
        getTexture("../engine/assets/grass/roughness.dds");

    grass_sys->normal = texture_mgr->
        getTexture("../engine/assets/grass/normal.dds");

    grass_sys->ambient_occlusion = texture_mgr->
        getTexture("../engine/assets/grass/ao.dds");

    grass_sys->translucency = texture_mgr->
        getTexture("../engine/assets/grass/translucency.dds");
    
    grass_sys->addGrassField(
        engine::GrassField(glm::vec3(75.0f, -10.5f, -5.0f),
                           glm::vec2(40.0f, 80.0f)));

    // grass_sys->addGrassField(
    //     engine::GrassField(glm::vec3(75.0f, -10.5f, -5.0f),
    //                        glm::vec2(2.0f, 2.0f)));
}

void Controller::processInput(Camera & camera,
                              engine::Postprocess & post_process, 
                              const float delta_time,
                              const engine::windows::Window & win)
{
    engine::TransformSystem * trans_system = engine::TransformSystem::getInstance();
    engine::MeshSystem * mesh_system = engine::MeshSystem::getInstance();
    
    RECT client_area = win.getClientSize();
    int width = client_area.right - client_area.left;
    int height = client_area.bottom - client_area.top;

    if (keys_log[KEY_W])
    {
        glm::vec3 offset = camera.getForward() * camera_movement_speed * delta_time;
        camera.addWorldPosition(offset);
    }
    if (keys_log[KEY_S])
    {
        glm::vec3 offset = camera.getForward() * -camera_movement_speed * delta_time;
        camera.addWorldPosition(offset);
    }
    if (keys_log[KEY_D])
    {
        glm::vec3 offset = camera.getRight() * camera_movement_speed * delta_time;
        camera.addWorldPosition(offset);
    }
    if (keys_log[KEY_A])
    {
        glm::vec3 offset = camera.getRight() * -camera_movement_speed * delta_time;
        camera.addWorldPosition(offset);
    }
    if (keys_log[KEY_Q])
    {
        glm::vec3 offset = camera.getUp() * -camera_movement_speed * delta_time;
        camera.addWorldPosition(offset);
    }
    if (keys_log[KEY_E])
    {
        glm::vec3 offset = camera.getUp() * camera_movement_speed * delta_time;
        camera.addWorldPosition(offset);
    }
    if (keys_log[KEY_SHIFT])
    {
        if (!is_accelerated)
        {
            camera_movement_speed *= 5.0f;
            is_accelerated = true;
        }
    }
    else
    {
        if(is_accelerated)
        {
            camera_movement_speed /= 5.0f;
            is_accelerated = false;
        }
    }
    if (keys_log[KEY_LMOUSE])
    {
        glm::vec2 delta_mouse = mouse - fixed_mouse;
        
        // delta_fixed_mouse normalized
        glm::vec2 speed(0);
        speed.x = (delta_mouse.y / float(height)) * camera_rotation_speed.x;
        speed.y = (delta_mouse.x / float(width)) * camera_rotation_speed.y;

        math::EulerAngles euler(speed.y * delta_time,
                                speed.x * delta_time,
                                0);

        camera.addRelativeAngles(euler);
    }
    if (keys_log[KEY_RMOUSE])
    {        
        camera.updateMatrices();

        glm::vec2 xy;
        xy.x = 2.0f * (mouse.x + 0.5f) / width - 1.0f;
        xy.y = 1.0f - 2.0f * (mouse.y + 0.5f) / height; // reversed

        math::Ray ray;
        ray.origin = camera.getPosition();
        ray.direction = camera.reproject(xy.x, xy.y) - ray.origin;

        math::MeshIntersection nearest;
        nearest.reset(0.0f);

        if (!object.is_grabbed)
        {           
            if (mesh_system->findIntersection(ray, nearest))
            {
                object.is_grabbed = true;
                object.transform_id = nearest.transform_id;
                object.t = nearest.t;
                object.pos = nearest.pos;
            }
        }
        else
        {
            auto & transform = trans_system->transforms[object.transform_id];
            
            glm::vec3 new_pos = camera.getPosition() + object.t * ray.direction;
            transform.position += (new_pos - object.pos);
            object.pos = new_pos;

            mesh_system->opaque_instances.updateInstanceBuffers();
            mesh_system->emissive_instances.updateInstanceBuffers();
        }
    }
    else
    {
        if (object.is_grabbed)
        {
            object.is_grabbed = false;
        }
    }
    if (keys_log[KEY_PLUS])
    {
        post_process.EV_100 += 2.0f * delta_time;
    }
    if (keys_log[KEY_MINUS])
    {
        post_process.EV_100 -= 2.0f * delta_time;
    }
    if (keys_log[KEY_R] && object.is_grabbed)
    {
        auto & transform = trans_system->transforms[object.transform_id];
        transform.rotation *= math::quatFromEuler(object_rotation_speed,
                                                  math::Basis());
    }
    else if (keys_log[KEY_T] && object.is_grabbed)
    {
        auto & transform = trans_system->transforms[object.transform_id];
        transform.rotation *= math::quatFromEuler(-object_rotation_speed,
                                                  math::Basis());
    }
    if (keys_log[KEY_N] && was_released[KEY_N])
    {
        was_released[KEY_N] = false;
        
        glm::vec3 position = camera.getPosition() +
                             30.0f * camera.getForward() +
                             glm::vec3(0.0f, -10.0f, 0.0f);
        
        spawnKnight(math::Transform(position,
                                    math::EulerAngles(0.0f, 0.0f, 0.0f),
                                    glm::vec3(10.0f, 10.0f, 10.0f)));
    }
}

