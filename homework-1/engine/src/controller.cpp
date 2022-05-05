#include "controller.hpp"

void Controller::init(const Window & win, Scene * scene)
{
    this->scene = scene;

    delta_mouse_x = 0;
    delta_mouse_y = 0;    

    RECT client = win.getSize();
    
    int width = client.right - client.left;
    int height = client.bottom - client.top;

    // set mouse at the client area center
    mouse_x = win.pos_x + width / 2;
    mouse_y = win.pos_y + height / 2;

    SetCursorPos(mouse_x, mouse_y);
        
    // reset keys
    for (int i = 0; i != KEYS_COUNT; ++i) keys_log[i] = false;
}

void Controller::initScene(Sphere sphere)
{
    scene->sphere = sphere;
}

void Controller::processInput()
{
    if (keys_log[KEY_W])
        scene->sphere.origin = scene->sphere.origin +
            vec3(0.0f, -10.0f, 0.0f);
    if (keys_log[KEY_A])
        scene->sphere.origin = scene->sphere.origin +
            vec3(-10.0f, 0.0f, 0.0f);
    if (keys_log[KEY_S])
        scene->sphere.origin = scene->sphere.origin +
            vec3(0.0f, 10.0f, 0.0f);
    if (keys_log[KEY_D])
        scene->sphere.origin = scene->sphere.origin + 
            vec3(10.0f, 0.0f, 0.0f);
    if (keys_log[KEY_RMOUSE])
    {
        scene->sphere.origin = scene->sphere.origin + 
            vec3(delta_mouse_x, delta_mouse_y, 0.0f);
        delta_mouse_x = 0;
        delta_mouse_y = 0;
    }
}

void Controller::calcMouseMovement(LPARAM lParam)
{
    int new_mouse_x = GET_X_LPARAM(lParam);
    int new_mouse_y = GET_Y_LPARAM(lParam);

    delta_mouse_x = new_mouse_x - mouse_x;
    delta_mouse_y = new_mouse_y - mouse_y;

    mouse_x = new_mouse_x;
    mouse_y = new_mouse_y;
}
