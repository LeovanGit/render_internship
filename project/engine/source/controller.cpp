#include "controller.hpp"

void Controller::init(engine::Scene & scene)
{
    this->scene = &scene;

    mouse = glm::vec2(0);
    fixed_mouse = glm::vec2(0);
    delta_fixed_mouse = glm::vec2(0);

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

    engine::ShaderManager * shader_mgr = engine::ShaderManager::getInstance();
    engine::TextureManager * tex_mgr = engine::TextureManager::getInstance();
    engine::ModelManager * model_mgr = engine::ModelManager::getInstance();
    engine::MeshSystem * mesh_system = engine::MeshSystem::getInstance();

    // CREATE SHADERS
    D3D11_INPUT_ELEMENT_DESC ied[] =
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

    shader_mgr->getShader("../engine/shaders/opaque.hlsl",
                          ied,
                          6);

    shader_mgr->getShader("../engine/shaders/post_processing.hlsl");
    
    // CREATE OBJECTS
    scene->sky.init(shader_mgr->getShader("../engine/shaders/skybox.hlsl"),
                    tex_mgr->getTexture("../engine/assets/skybox.dds"));
    
    initKnight(math::Transform(glm::vec3(10.0f, -10.0f, 0.0f),
                               math::EulerAngles(90.0f, 0.0f, 0.0f),
                               glm::vec3(10.0f, 10.0f, 10.0f)));

    initKnight(math::Transform(glm::vec3(-10.0f, -10.0f, 0.0f),
                               math::EulerAngles(-90.0f, 0.0f, 0.0f),
                               glm::vec3(10.0f, 10.0f, 10.0f)));

    initCube("../engine/assets/rubik_cube.dds",
             math::Transform(glm::vec3(0.0f, 0.0f, 10.0f),
                             math::EulerAngles(0.0f, 45.0f, 45.0f),
                             glm::vec3(4.0f, 4.0f, 4.0f)));
    
    initCube("../engine/assets/prototype.dds",
             math::Transform(glm::vec3(0.0f, -11.0f, 10.0f),
                             math::EulerAngles(0.0f, 0.0f, 0.0f),
                             glm::vec3(4.0f, 4.0f, 4.0f)));

    initPlane("../engine/assets/prototype_grid.dds",
              math::Transform(glm::vec3(0.0f, -10.0f, 0.0f),
                              math::EulerAngles(0.0f, 90.0f, 0.0f),
                              glm::vec3(100.0f, 100.0f, 100.0f)));
}

void Controller::initKnight(const math::Transform & transform)
{    
    engine::TextureManager * tex_mgr = engine::TextureManager::getInstance();
    engine::ModelManager * model_mgr = engine::ModelManager::getInstance();
    engine::MeshSystem * mesh_system = engine::MeshSystem::getInstance();

    std::vector<oi::Material> materials =
    {
        oi::Material(tex_mgr->
                     getTexture("../engine/assets/Knight/dds/Fur_BaseColor.dds")),
        oi::Material(tex_mgr->
                     getTexture("../engine/assets/Knight/dds/Legs_BaseColor.dds")),
        oi::Material(tex_mgr->
                     getTexture("../engine/assets/Knight/dds/Torso_BaseColor.dds")),
        oi::Material(tex_mgr->
                     getTexture("../engine/assets/Knight/dds/Head_BaseColor.dds")),
        oi::Material(tex_mgr->
                     getTexture("../engine/assets/Knight/dds/Eye_BaseColor.dds")),
        oi::Material(tex_mgr->
                     getTexture("../engine/assets/Knight/dds/Helmet_BaseColor.dds")),
        oi::Material(tex_mgr->
                     getTexture("../engine/assets/Knight/dds/Skirt_BaseColor.dds")),
        oi::Material(tex_mgr->
                     getTexture("../engine/assets/Knight/dds/Cape_BaseColor.dds")),
        oi::Material(tex_mgr->
                     getTexture("../engine/assets/Knight/dds/Glove_BaseColor.dds")),
    };

    mesh_system->addInstance(model_mgr->getModel("../engine/assets/Knight/Knight.fbx"),
                             materials,
                             oi::Instance(transform.toMat4()));
}

void Controller::initCube(const std::string & texture_path,
                          const math::Transform & transform)
{
    engine::TextureManager * tex_mgr = engine::TextureManager::getInstance();
    engine::ModelManager * model_mgr = engine::ModelManager::getInstance();
    engine::MeshSystem * mesh_system = engine::MeshSystem::getInstance();

    std::vector<oi::Material> materials =
    {
        oi::Material(tex_mgr->getTexture(texture_path)),
    };

    mesh_system->addInstance(model_mgr->getDefaultCube("cube"),
                             materials,
                             oi::Instance(transform.toMat4()));
}

void Controller::initPlane(const std::string & texture_path,
                           const math::Transform & transform)
{
    engine::TextureManager * tex_mgr = engine::TextureManager::getInstance();
    engine::ModelManager * model_mgr = engine::ModelManager::getInstance();
    engine::MeshSystem * mesh_system = engine::MeshSystem::getInstance();

    std::vector<oi::Material> materials =
    {
        oi::Material(tex_mgr->getTexture(texture_path)),
    };

    mesh_system->addInstance(model_mgr->getDefaultPlane("plane"),
                             materials,
                             oi::Instance(transform.toMat4()));
}

void Controller::processInput(Camera & camera,
                              const float delta_time,
                              const engine::windows::Window & win)
{
    RECT client_area = win.getClientSize();
    int width = client_area.right - client_area.left;
    int height = client_area.bottom - client_area.top;

    // was any user input
    // for (int i = 0; i != KEYS_COUNT; ++i)
    // {
    //     if(keys_log[i] == true)
    //     {
    //         if (scene->is_image_ready)
    //         {
    //             scene->is_global_illumination = false;
    //             scene->is_image_ready = false;
    //         }
            
    //         break;
    //     }
    // }

    if (keys_log[KEY_W])
    {
        glm::vec3 offset = camera.getForward() * movement_speed * delta_time;
        camera.addWorldPosition(offset);
    }
    if (keys_log[KEY_S])
    {
        glm::vec3 offset = camera.getForward() * -movement_speed * delta_time;
        camera.addWorldPosition(offset);
    }
    if (keys_log[KEY_D])
    {
        glm::vec3 offset = camera.getRight() * movement_speed * delta_time;
        camera.addWorldPosition(offset);
    }
    if (keys_log[KEY_A])
    {
        glm::vec3 offset = camera.getRight() * -movement_speed * delta_time;
        camera.addWorldPosition(offset);
    }
    if (keys_log[KEY_Q])
    {
        glm::vec3 offset = camera.getUp() * -movement_speed * delta_time;
        camera.addWorldPosition(offset);
    }
    if (keys_log[KEY_E])
    {
        glm::vec3 offset = camera.getUp() * movement_speed * delta_time;
        camera.addWorldPosition(offset);
    }
    if (keys_log[KEY_SHIFT])
    {
        if (!is_accelerated)
        {
            movement_speed *= 5.0f;
            is_accelerated = true;
        }
    }
    else
    {
        if(is_accelerated)
        {
            movement_speed /= 5.0f;
            is_accelerated = false;
        }
    }
    if (keys_log[KEY_LMOUSE])
    {	
        // delta_fixed_mouse normalized
        glm::vec2 speed(0);
        speed.x = (delta_fixed_mouse.y / float(height)) * rotation_speed.x;
        speed.y = (delta_fixed_mouse.x / float(width)) * rotation_speed.y;

        math::EulerAngles euler(speed.y * delta_time,
                                speed.x * delta_time,
                                0);

        camera.addRelativeAngles(euler);
    }
    // if (keys_log[KEY_RMOUSE])
    // {
    //     camera.updateMatrices();

    //     glm::vec2 xy;
    //     xy.x = 2.0f * (mouse.x + 0.5f) / width - 1.0f;           
    //     xy.y = 1.0f - 2.0f * (mouse.y + 0.5f) / height; // reversed

    //     math::Ray ray;
    //     ray.origin = camera.getPosition();
    //     ray.direction = camera.reproject(xy.x, xy.y) - ray.origin;

    //     if (!object.is_grabbed)
    //     {
    //         Scene::IntersectionQuery iq;
    //         iq.nearest.reset();
    //         iq.mover = &object.mover;

    //         if (scene->findIntersection(ray, iq))
    //         {
    //             if (iq.mover->get())
    //             {
    //                 object.t = iq.nearest.t;
    //                 object.point = iq.nearest.point;
    //                 object.is_grabbed = true;
    //             }
    //         }
    //     }
    //     else
    //     {
    //         glm::vec3 dest = camera.getPosition() + object.t * ray.direction;
    //         object.mover->move(dest - object.point);
    //         object.point = dest;
    //     }
    // }
    // else
    // {
    //     if (object.is_grabbed)
    //     {
    //         object.is_grabbed = false;
    //         object.mover.reset();
    //     }
    // }
    if (keys_log[KEY_PLUS])
    {
        camera.EV_100 += 2.0f * delta_time;
    }
    if (keys_log[KEY_MINUS])
    {
        camera.EV_100 -= 2.0f * delta_time;
    }
    // if (keys_log[KEY_G] && 
    //     !scene->is_global_illumination && 
    //     !scene->is_image_ready)
    // {
    //     scene->is_global_illumination = true;
    //     scene->is_image_ready = false;
    // }
    // if (keys_log[KEY_R])
    // {
    //     if (was_released[KEY_R])
    //     {
    //         scene->is_smooth_reflection = !scene->is_smooth_reflection;
    //         was_released[KEY_R] = false;
    //     }
    // }
}

void Controller::calcMouseMovement(LPARAM lParam)
{
    mouse = glm::vec2(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

    if (keys_log[KEY_LMOUSE])
    {
        delta_fixed_mouse.x = mouse.x - fixed_mouse.x;
        delta_fixed_mouse.y = mouse.y - fixed_mouse.y;
    }
}
