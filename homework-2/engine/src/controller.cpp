#include "controller.hpp"

void Controller::init(const Window & win, Scene * scene)
{
    this->scene = scene;

    mouse_x = 0;
    mouse_y = 0;

    delta_mouse_x = 0;
    delta_mouse_y = 0;    

    for (int i = 0; i != KEYS_COUNT; ++i) keys_log[i] = false;
}

void Controller::initScene(std::vector<ColoredSphere> & c_spheres,
                           std::vector<PointLight> & p_lights)
{
    scene->c_spheres = c_spheres;
    scene->p_lights = p_lights;
}

// void Controller::processInput()
// {
//     if (keys_log[KEY_W])
//         scene->csphere.sphere.origin = scene->csphere.sphere.origin + vec3(0.0f, -10.0f, 0.0f);
//     if (keys_log[KEY_A])
//         scene->csphere.sphere.origin = scene->csphere.sphere.origin + vec3(-10.0f, 0.0f, 0.0f);
//     if (keys_log[KEY_S])
//         scene->csphere.sphere.origin = scene->csphere.sphere.origin + vec3(0.0f, 10.0f, 0.0f);
//     if (keys_log[KEY_D])
//         scene->csphere.sphere.origin = scene->csphere.sphere.origin + vec3(10.0f, 0.0f, 0.0f);
//     if (keys_log[KEY_RMOUSE])
//     {
//         scene->csphere.sphere.origin = scene->csphere.sphere.origin +
//             vec3(delta_mouse_x, delta_mouse_y, 0.0f);
//         delta_mouse_x = 0;
//         delta_mouse_y = 0;
//     }
// }

void Controller::calcMouseMovement(LPARAM lParam)
{
    int new_mouse_x = GET_X_LPARAM(lParam);
    int new_mouse_y = GET_Y_LPARAM(lParam);

    if (keys_log[KEY_RMOUSE])
    {
        delta_mouse_x = new_mouse_x - mouse_x;
        delta_mouse_y = new_mouse_y - mouse_y;
    }

    mouse_x = new_mouse_x;
    mouse_y = new_mouse_y;
}
