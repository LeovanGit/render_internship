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
    // SPHERES GRID
    // for (int row = 0; row != 7; ++row)
    // {
    //     for (int col = 0; col != 7; ++col)
    //     {
    //         float roughness = 0.01f + 0.99f / 6.0f * col;
    //         float metalness = 1.0f - 1.0f / 6.0f * row;

    //         scene->spheres.push_back(
    //             Scene::Sphere(100.0f,
    //                           glm::vec3(-660.0f + 220.0f * col,
    //                                     660.0f - 220.0f * row,
    //                                     0),
    //                           Material(
    //                               glm::vec3(1.0f, 0.0f, 0.0f),
    //                               1.0f - roughness,
    //                               metalness,
    //                               glm::vec3(0.0f))));
    //     }
    // }

            scene->spheres.push_back(
                Scene::Sphere(100.0f,
                              glm::vec3(0,
                                        0,
                                        0),
                              Material(
                                  glm::vec3(1.0f, 0.0f, 0.0f),
                                  1.0f,
                                  0.0f,
                                  glm::vec3(0.0f))));

    // LIGHTS
    scene->p_lights.push_back(Scene::PointLight(
                                  glm::vec3(1000.0f, 1000.0f, -1000.0f),
                                  35.0f,
                                  glm::vec3(1.0f, 1.0f, 1.0f),
                                  10000.0f));

    // scene->p_lights.push_back(Scene::PointLight(
    //                               glm::vec3(0.0f, -900.0f, -500.0f),
    //                               15.0f,
    //                               glm::vec3(1.0f),
    //                               10000.0f));

    // scene->d_lights.push_back(Scene::DirectionalLight(
    //                               glm::vec3(1.0f, 1.0f, 1.0f),
    //                               5.0f,
    //                               glm::vec3(1.0f, -1.0f, 1.0f)));

    // scene->s_lights.push_back(Scene::SpotLight(
    //                               glm::vec3(1000.0f, 0.0f, -300.0f),
    //                               20.0f,
    //                               glm::vec3(1.0f, 0.5f, 0.1f),
    //                               10000.0f,
    //                               30.0f,
    //                               glm::vec3(0.0f, -1.0f, 1.0f)));

    // ADDITIONAL OBJECTS
    // scene->planes.push_back(Scene::Plane(glm::vec3(0.0f, 1.0f, 0.0f),
    //                                      glm::vec3(0.0f, -1000.0f, 0.0f),
    //                                      Material(
    //                                          glm::vec3(0.3f),
    //                                          0.2f,
    //                                          0.0f,
    //                                          glm::vec3(0.0f))));

    // scene->cubes.push_back(Scene::Cube(glm::vec3(0, 0, -300.0f),
    //                                    math::EulerAngles(0, 0, 0),
    //                                    glm::vec3(100.0f),
    //                                    Material(
    //                                        glm::vec3(0, 1.0f, 0),
    //                                        0.95f,
    //                                        1.0f,
    //                                        glm::vec3(0.0f))));
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
    if (keys_log[KEY_RMOUSE])
    {
        camera.updateMatrices();

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
    if (keys_log[KEY_PLUS])
    {
        camera.EV_100 += 0.1f;
    }
    if (keys_log[KEY_MINUS])
    {
        camera.EV_100 -= 0.1f;
    }
    if (keys_log[KEY_R])
    {
        // mirror mode
    }
    if (keys_log[KEY_G])
    {
        // rendering mode
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
