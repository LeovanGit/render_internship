#include "controller.hpp"

void Controller::init(const Window & win, Scene * scene)
{
    this->scene = scene;

    mouse.x = 0;
    mouse.y = 0;

    fixed_mouse.x = 0;
    fixed_mouse.y = 0;

    delta_mouse.x = 0;
    delta_mouse.y = 0;

    for (int i = 0; i != KEYS_COUNT; ++i) keys_log[i] = false;
}

void Controller::initScene(std::vector<ColoredSphere> & c_spheres,
                           std::vector<Plane> & planes,
                           std::vector<PointLight> & p_lights)
{
    scene->c_spheres = c_spheres;
    scene->planes = planes;
    scene->p_lights = p_lights;
}

void Controller::processInput(Camera & camera, float delta_time)
{
    if (keys_log[KEY_W])
        camera.addPosition(camera.getForward() * delta_time * MOVEMENT_SPEED);
    if (keys_log[KEY_S])
        camera.addPosition(camera.getForward() * -delta_time * MOVEMENT_SPEED);
    if (keys_log[KEY_D])
        camera.addPosition(camera.getRight() * delta_time * MOVEMENT_SPEED);
    if (keys_log[KEY_A])
        camera.addPosition(camera.getRight() * -delta_time * MOVEMENT_SPEED);
    if (keys_log[KEY_SPACE])
        camera.addPosition(camera.getUp() * delta_time * MOVEMENT_SPEED);
    if (keys_log[KEY_CTRL])
        camera.addPosition(camera.getUp() * -delta_time * MOVEMENT_SPEED);
    if (keys_log[KEY_LMOUSE])
    {
       camera.addAngles(glm::vec3(delta_mouse.y / 100 * ROTATIONAL_SPEED *
                                                        delta_time,
                                  delta_mouse.x / 100 * ROTATIONAL_SPEED  * 
                                                        delta_time,
                                  0));
    }
    if (keys_log[KEY_Q])
    {
       camera.addAngles(glm::vec3(0,
                                  0,
                                  ROTATIONAL_SPEED * delta_time));
    }
    if (keys_log[KEY_E])
    {
        camera.addAngles(glm::vec3(0,
                                   0,
                                   -ROTATIONAL_SPEED * delta_time));
    }
}

void Controller::calcMouseMovement(LPARAM lParam)
{
    mouse.x = GET_X_LPARAM(lParam);
    mouse.y = GET_Y_LPARAM(lParam);

    if (keys_log[KEY_LMOUSE])
    {
        delta_mouse.x = mouse.x - fixed_mouse.x;
        delta_mouse.y = mouse.y - fixed_mouse.y;
    }
}
