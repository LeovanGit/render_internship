#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <vector>
#include <windows.h>
#include <windowsx.h>

#include "sphere.hpp"

#define HEX_WHITE 0xFFFFFF
#define HEX_BLACK 0x000000

#define KEYS_COUNT 254 // 254 keys defined in WinAPI
#define KEY_W 87
#define KEY_A 65
#define KEY_S 83
#define KEY_D 68
#define KEY_RMOUSE 2

class Window
{
public:
    Window() = default;

    Window(HINSTANCE hInstance,
           int pos_x, int pos_y,
           int client_width, int client_height);

    void resize(int width, int height);

    std::vector<int> & getPixels();

    RECT getSize() const;

    void flush();

    HWND handle;
    HDC hdc;

    int pos_x;
    int pos_y;

private:    
    int client_width;
    int client_height;

    BITMAPINFO bmi;
    std::vector<int> pixels;
};

class Scene
{
public:
    explicit Scene(Sphere sphere = Sphere(100, vec3(0, 0, 0)));

    void render(Window win);

    Sphere sphere;
};

class Controller
{
public:
    Controller() = default;

    Controller(Window win, Scene * scene);

    void processInput();

    void calcMouseMovement(LPARAM lParam);

    Scene * scene;

    int mouse_x;
    int mouse_y;

    int delta_mouse_x;
    int delta_mouse_y;
    
    bool keys_log[KEYS_COUNT];
};

#endif
