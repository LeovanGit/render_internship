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
                           std::vector<ColoredPlane> & c_planes,
                           std::vector<Cube> & c_cubes,
                           std::vector<PointLight> & p_lights,
                           std::vector<DirectionalLight> & d_lights,
                           std::vector<SpotLight> & s_lights)
{
    scene->c_spheres = c_spheres;
    scene->c_planes = c_planes;
    scene->c_cubes = c_cubes;
    scene->p_lights = p_lights;
    scene->d_lights = d_lights;
    scene->s_lights = s_lights;
}

void Controller::processInput(Camera & camera, float delta_time)
{
    if (keys_log[KEY_W])
        camera.addWorldPosition(camera.getForward() *
                                delta_time * MOVEMENT_SPEED);
    if (keys_log[KEY_S])
        camera.addWorldPosition(camera.getForward() * 
                                delta_time * -MOVEMENT_SPEED);
    if (keys_log[KEY_D])
        camera.addWorldPosition(camera.getRight() *
                                delta_time * MOVEMENT_SPEED);
    if (keys_log[KEY_A])
        camera.addWorldPosition(camera.getRight() *
                                delta_time * -MOVEMENT_SPEED);
    if (keys_log[KEY_SPACE])
        camera.addWorldPosition(camera.getUp() *
                                delta_time * MOVEMENT_SPEED);
    if (keys_log[KEY_CTRL])
        camera.addWorldPosition(camera.getUp() *
                                delta_time * -MOVEMENT_SPEED);
    if (keys_log[KEY_LMOUSE])
    {
        glm::vec3 angles;
        angles.x = delta_mouse.y / 100 * ROTATIONAL_SPEED * delta_time;
        angles.y = delta_mouse.x / 100 * ROTATIONAL_SPEED * delta_time;
        angles.z = 0;

        camera.addRelativeAngles(angles);
    }
    if (keys_log[KEY_Q])
    {
        camera.addRelativeAngles(glm::vec3(0,
                                           0,
                                           ROTATIONAL_SPEED * delta_time));
    }
    if (keys_log[KEY_E])
    {
        camera.addRelativeAngles(glm::vec3(0,
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
