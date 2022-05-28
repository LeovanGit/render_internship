#include "controller.hpp"

void Controller::init(Scene * scene)
{
    this->scene = scene;

    mouse = glm::vec2(0);
    fixed_mouse = glm::vec2(0);
    delta_fixed_mouse = glm::vec2(0);

    for (int i = 0; i != KEYS_COUNT; ++i) keys_log[i] = false;
}

void Controller::initScene()
{
    // CREATE MATERIALS
    enum Materials
    {
        GLOSSY_BLUE,
        MATTE_GREY,
        MATTE_RED,
        MATTE_WHITE,
        HALF_GLOSSY_GREEN,
        GLOSSY_RED,
    };

    std::vector<Material> materials;
    // glossy blue
    materials.push_back(Material(glm::vec3(0.4f, 0.0f, 0.0f),
                                 1.0f,
                                 128.0f,
                                 glm::vec3(0.0f, 0.0f, 0.0f)));
    // matte grey
    materials.push_back(Material(glm::vec3(0.5f, 0.5f, 0.5f),
                                 0,
                                 0,
                                 glm::vec3(0, 0, 0)));
    // matte red
    materials.push_back(Material(glm::vec3(0.0f, 0.0f, 0.8f),
                                 0.2f,
                                 16.0f,
                                 glm::vec3(0.0f, 0.0f, 0.0f)));
    // matte white
    materials.push_back(Material(glm::vec3(1.0f, 1.0f, 1.0f),
                                 0.2f,
                                 16.0f,
                                 glm::vec3(0.0f, 0.0f, 0.0f)));

    // half-glossy green
    materials.push_back(Material(glm::vec3(0.0f, 0.4f, 0.0f),
                                 0.6f,
                                 72.0f,
                                 glm::vec3(0.0f, 0.0f, 0.0f)));
    // glossy red
    materials.push_back(Material(glm::vec3(0.0f, 0.0f, 0.8f),
                                 1.0f,
                                 128.0f,
                                 glm::vec3(0.0f, 0.0f, 0.0f)));

    // CREATE PLANES
    // floor
    scene->planes.push_back(Scene::Plane(glm::vec3(0, 1.0f, 0),
                                         glm::vec3(0, -100.0f, 0),
                                         materials[MATTE_GREY]));

    // CREATE SPHERES
    scene->spheres.push_back(Scene::Sphere(80.0f,
                                           glm::vec3(0, -20.0f, 0),
                                           materials[GLOSSY_BLUE]));

    scene->spheres.push_back(Scene::Sphere(100.0f,
                                           glm::vec3(-180.0f, 200.0f, 100.0f),
                                           materials[MATTE_RED]));

    scene->spheres.push_back(Scene::Sphere(120.0f,
                                           glm::vec3(300.0f, 20.0f, 100.0f),
                                           materials[HALF_GLOSSY_GREEN]));

    // CREATE CUBES
    scene->cubes.push_back(Scene::Cube(glm::vec3(-180.0f, 0, 100.0f),
                                       glm::vec3(0.0f, 45.0f, 0.0f),
                                       glm::vec3(100.0f, 100.0f, 100.0f),
                                       materials[MATTE_WHITE]));

    scene->cubes.push_back(Scene::Cube(glm::vec3(-350.0f, -25.0f, -70.0f),
                                       glm::vec3(0.0f, 45.0f, 0.0f),
                                       glm::vec3(75.0f, 75.0f, 75.0f),
                                       materials[GLOSSY_RED]));

    // CREATE LIGHTS
    scene->p_lights.push_back(Scene::PointLight(glm::vec3(-400.0, 250.0f, -300.0f),
                                                350.0f,
                                                glm::vec3(0.8f)));
    
    scene->d_lights.push_back(Scene::DirectionalLight(
                                  glm::normalize(glm::vec3(1.0f, -1.0f, 1.0f)),
                                  glm::vec3(0.15f)));

    scene->s_lights.push_back(Scene::SpotLight(
                                  glm::vec3(200, 200, -300),
                                  400.0f,
                                  55.0f,
                                  glm::normalize(glm::vec3(0, -1.0f, 1.0f)),
                                  glm::vec3(0.1f, 0.5f, 1.0f)));
}

void Controller::processInput(Camera & camera,
                              const float delta_time,
                              const Window & win)
{
    RECT client_area = win.getClientSize();
    int width = client_area.right - client_area.left;
    int height = client_area.bottom - client_area.top;

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
    if (keys_log[KEY_SPACE])
    {
        glm::vec3 offset = camera.getUp() * movement_speed * delta_time;
        camera.addWorldPosition(offset);
    }
    if (keys_log[KEY_CTRL])
    {
        glm::vec3 offset = camera.getUp() * -movement_speed * delta_time;
        camera.addWorldPosition(offset);
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
    if (keys_log[KEY_Q])
    {
        math::EulerAngles euler(0,
                                0,
                                rotation_speed.z * delta_time);
                
        camera.addRelativeAngles(euler);
    }
    if (keys_log[KEY_E])
    {
        math::EulerAngles euler(0,
                                0,
                                -rotation_speed.z * delta_time);
                
        camera.addRelativeAngles(euler);
    }
    if (keys_log[KEY_RMOUSE])
    {
        glm::vec2 xy;
        xy.x = 2.0f * (mouse.x + 0.5f) / width - 1.0f;           
        xy.y = 1.0f - 2.0f * (mouse.y + 0.5f) / height; // reversed

        math::Ray ray;
        ray.origin = camera.getPosition();
        ray.direction = camera.generateWorldPointFromCS(xy.x, xy.y) -
                        ray.origin;

        if (!object.is_grabbed)
        {
            Scene::IntersectionQuery iq;
            iq.nearest.reset();
            iq.mover = &object.mover;

            if (scene->findIntersection(ray, iq))
            {
                if (iq.mover->get())
                {
                    object.t = iq.nearest.t;
                    object.point = iq.nearest.point;
                    object.is_grabbed = true;
                }
            }
        }
        else
        {
            glm::vec3 dest = camera.getPosition() + object.t * ray.direction;
            object.mover->move(dest - object.point);
            object.point = dest;
        }
    }
    else
    {
        if (object.is_grabbed)
        {
            object.is_grabbed = false;
            object.mover.reset();
        }
    }
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
