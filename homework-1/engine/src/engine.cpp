#include "engine.hpp"

Window::Window(HINSTANCE hInstance,
               int pos_x, int pos_y,
               int client_width, int client_height) :
    pos_x(pos_x), pos_y(pos_y),
    client_width(client_width), client_height(client_height)
{
    // calculate the window size consided on client area
    RECT window = {0, 0, client_width, client_height};
    AdjustWindowRect(&window, WS_OVERLAPPEDWINDOW, FALSE);

    handle = CreateWindowEx(0,
                            "window_class_1",
                            "raytracing",
                            WS_OVERLAPPEDWINDOW,
                            pos_x,
                            pos_y,
                            window.right - window.left,
                            window.bottom - window.top,
                            NULL,
                            NULL,
                            hInstance,
                            NULL);

    hdc = GetDC(handle);

    // BITMAPINFO
    bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
    bmi.bmiHeader.biWidth = client_width;
    bmi.bmiHeader.biHeight = -client_height; // inverted img
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32; // 32 bits to represent pixel
    bmi.bmiHeader.biCompression = BI_RGB;

    pixels.resize(client_width * client_height);
}

void Window::resize(int width, int height)
{
    client_width = width;
    client_height = height;

    pixels.resize(client_width * client_height);

    bmi.bmiHeader.biWidth = client_width;
    bmi.bmiHeader.biHeight = -client_height;
}

std::vector<int> & Window::getPixels() { return pixels; }

RECT Window::getSize() const
{
    RECT client;
    GetClientRect(handle, &client);
    return client;
}

void Window::flush()
{        
    SetDIBitsToDevice(hdc,
                      0,
                      0,
                      client_width,
                      client_height,
                      0,
                      0,
                      0,
                      client_height,
                      pixels.data(),
                      &bmi,
                      DIB_RGB_COLORS);
}

Scene::Scene(Sphere sphere) : sphere(sphere)
{}

void Scene::render(Window win)
{ 
    RECT client = win.getSize();
    float width = client.right - client.left;
    float height = client.bottom - client.top;
        
    std::vector<int> & pixels = win.getPixels();

    // orthographic: all rays collinear
    vec3 ray_direction(0, 0, 1);

    for (int y = 0; y != height; ++y)        
    {
        for (int x = 0; x != width; ++x)
        {
            // new coord system with (0, 0) in client area center
            int new_x = x - width / 2;
            int new_y = y - height / 2;

            vec3 ray_origin(new_x, new_y, -1);

            if (sphere.isIntersect(ray_origin, ray_direction))
            {
                pixels[y * width + x] = HEX_WHITE;
            }
            else
            {
                pixels[y * width + x] = HEX_BLACK;
            }
        }
    }

    win.flush();
}

Controller::Controller(Window win, Scene * scene) :
    delta_mouse_x(0),
    delta_mouse_y(0),
    scene(scene)
{
    *scene = Scene(Sphere(100, vec3(0, 0, 0)));

    RECT client = win.getSize();
    float width = client.right - client.left;
    float height = client.bottom - client.top;

    // set mouse at the client area center
    mouse_x = win.pos_x + width / 2;
    mouse_y = win.pos_y + height / 2;

    SetCursorPos(mouse_x, mouse_y);
        
    // reset keys
    for (int i = 0; i != KEYS_COUNT; ++i) keys_log[i] = false;
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
