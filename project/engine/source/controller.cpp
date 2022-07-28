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
    };

    shader_mgr->registerShader("opaque",
                               L"../engine/shaders/",
                               L"opaque.hlsl",
                               ied);

    shader_mgr->registerShader("skybox",
                               L"../engine/shaders/",
                               L"skybox.hlsl",
                               nullptr);

    shader_mgr->registerShader("floor",
                               L"../engine/shaders/",
                               L"floor.hlsl",
                               nullptr);
    
    // CREATE TEXTURES
    tex_mgr->registerTexture("cube", L"../engine/assets/rubik_cube.dds");

    tex_mgr->registerTexture("cape", L"../engine/assets/Knight/dds/Cape_BaseColor.dds");
    tex_mgr->registerTexture("eye", L"../engine/assets/Knight/dds/Eye_BaseColor.dds");
    tex_mgr->registerTexture("fur", L"../engine/assets/Knight/dds/Fur_BaseColor.dds");
    tex_mgr->registerTexture("glove", L"../engine/assets/Knight/dds/Glove_BaseColor.dds");
    tex_mgr->registerTexture("head", L"../engine/assets/Knight/dds/Head_BaseColor.dds");
    tex_mgr->registerTexture("helmet", L"../engine/assets/Knight/dds/Helmet_BaseColor.dds");
    tex_mgr->registerTexture("legs", L"../engine/assets/Knight/dds/Legs_BaseColor.dds");
    tex_mgr->registerTexture("skirt", L"../engine/assets/Knight/dds/Skirt_BaseColor.dds");
    tex_mgr->registerTexture("torso", L"../engine/assets/Knight/dds/Torso_BaseColor.dds");
    
    tex_mgr->registerTexture("skybox", L"../engine/assets/skybox.dds");
    tex_mgr->registerTexture("floor", L"../engine/assets/prototype_grid.dds");

    // CREATE SKY
    scene->m_sky.init(shader_mgr->getShader("skybox"),
                      tex_mgr->getTexture("skybox"));

    // CREATE MODELS
    model_mgr->registerModel("knight", "../engine/assets/Knight/Knight.fbx");
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
        camera.EV_100 += 6.0f * delta_time;
    }
    if (keys_log[KEY_MINUS])
    {
        camera.EV_100 -= 6.0f * delta_time;
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
