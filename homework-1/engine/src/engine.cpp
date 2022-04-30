#include <stdint.h>
#include <windows.h>
#include <windowsx.h>

#include "math.hpp"
#include "sphere.hpp"

#include <iostream>

#define WIN_POS_X 300
#define WIN_POS_Y 300

#define KEY_W 87
#define KEY_A 65
#define KEY_S 83
#define KEY_D 68
#define KEY_RMOUSE 2

#define HEX_WHITE 0xFFFFFF
#define HEX_BLACK 0x000000

bool keys_log[254]; // 254 keys defined in WinAPI

int client_width = 600;
int client_height = 600;

int mouse_x = WIN_POS_X + client_width / 2;
int mouse_y = WIN_POS_Y + client_height / 2;
int delta_mouse_x = 0;
int delta_mouse_y = 0;

float delta_time = 0;
float prev_time = GetTickCount();

Sphere sphere(100, vec3(0, 0, 0));

LRESULT CALLBACK WindowProc(HWND hWnd,
                            UINT message,
                            WPARAM wParam,
                            LPARAM lParam);

void move();

void calc_delta_time()
{
    float time = GetTickCount();
    delta_time = time - prev_time;
    prev_time = time;
}

void render(HWND hWnd)
{
    HDC hdc = GetDC(hWnd);
 
    BITMAPINFO bmi;
    bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
    bmi.bmiHeader.biWidth = client_width;
    bmi.bmiHeader.biHeight = -client_height; // inverted img
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32; // 32 bits to represent pixel
    bmi.bmiHeader.biCompression = BI_RGB;    
   
    // 32 bits each pixel
    uint32_t * pixels = new uint32_t[client_width * client_height * 4];

    // orthographic: all rays collinear
    vec3 ray_direction(0, 0, 1);

    for (int x = 0; x != client_width; ++x)
    {
        for (int y = 0; y != client_height; ++y)
        {
            // new coord system with (0, 0) in client area center
            int new_x = x - client_width / 2;
            int new_y = y - client_height / 2;

            vec3 ray_origin(new_x, new_y, -1);
            vec3 ray_to_sphere_origin = ray_origin - sphere.getOrigin();

            // quadratic equation coeff
            float a = 1; // dot(ray_direction, ray_direction)
            float b = 2.0f * dot(ray_direction, ray_to_sphere_origin);
            float c = dot(ray_to_sphere_origin, ray_to_sphere_origin) -
                      sphere.getRadius() * sphere.getRadius();

            float discriminant = b * b - 4 * a * c;

            if (discriminant >= 0)
            {
                // t1 = (-b - sqrt(discriminant)) / (2.0f * a);
                // t2 = (-b + sqrt(discriminant)) / (2.0f * a);
                pixels[y * client_width + x] = HEX_WHITE;
            }
            else 
            {
                pixels[y * client_width + x] = HEX_BLACK;
            }
        }
    }

    // draw
    SetDIBitsToDevice(hdc,
                      0,
                      0,
                      client_width,
                      client_height,
                      0,
                      0,
                      0,
                      client_height,
                      pixels,
                      &bmi,
                      DIB_RGB_COLORS);

    // clear memory
    delete [] pixels;
    pixels = nullptr;
}

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    // REGISTER THE WINDOW CLASS
    HWND hWnd;
    WNDCLASSEX win_class;

    // clear the window class:
    ZeroMemory(&win_class, sizeof(WNDCLASSEX));

    // window class properties
    win_class.cbSize = sizeof(WNDCLASSEX);
    win_class.style = CS_HREDRAW | CS_VREDRAW;
    win_class.lpfnWndProc = WindowProc;
    win_class.hInstance = hInstance;
    win_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    win_class.hbrBackground = (HBRUSH)COLOR_WINDOW;
    win_class.lpszClassName = "window_class_1";

    RegisterClassEx(&win_class);

    // calculate the window size consided on client area
    RECT window = {0, 0, client_width, client_height};
    AdjustWindowRect(&window, WS_OVERLAPPEDWINDOW, FALSE);

    // CREATE THE WINDOW (using window class above)
    hWnd = CreateWindowEx(NULL,
                          "window_class_1",
                          "raytracing",
                          WS_OVERLAPPEDWINDOW,
                          WIN_POS_X,
                          WIN_POS_Y,
                          window.right - window.left,
                          window.bottom - window.top,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hWnd, nCmdShow);

    // set mouse at the client area center
    SetCursorPos(mouse_x, mouse_y);

    // MAIN LOOP (EVENT HANDLING)
    MSG msg;

    while(true)
    {
        // take another msg from the event queue
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            // translate key-pressed messages into the right format
            TranslateMessage(&msg);

            // send the message to the WindowProc function
            DispatchMessage(&msg);

            // quit
            if (msg.message == WM_QUIT) break;
        }

        calc_delta_time();
        move();
        render(hWnd);
    }

    // return this part of the WM_QUIT message to Windows
    return msg.wParam;
}

void calc_mouse_movement(LPARAM lParam)
{
    int new_mouse_x = GET_X_LPARAM(lParam);
    int new_mouse_y = GET_Y_LPARAM(lParam);

    delta_mouse_x = new_mouse_x - mouse_x;
    delta_mouse_y = new_mouse_y - mouse_y;

    mouse_x = new_mouse_x;
    mouse_y = new_mouse_y;
}

void move()
{
    if (keys_log[KEY_W])
        sphere.setOrigin(sphere.getOrigin() +
                         vec3(0.0f, -0.5f, 0.0f) * delta_time);
    if (keys_log[KEY_A])
        sphere.setOrigin(sphere.getOrigin() +
                         vec3(-0.5f, 0.0f, 0.0f) * delta_time);
    if (keys_log[KEY_S])
        sphere.setOrigin(sphere.getOrigin() +
                         vec3(0.0f, 0.5f, 0.0f) * delta_time);
    if (keys_log[KEY_D])
        sphere.setOrigin(sphere.getOrigin() + 
                         vec3(0.5f, 0.0f, 0.0f) * delta_time);
    if (keys_log[KEY_RMOUSE])
    {
        sphere.setOrigin(sphere.getOrigin() + 
                         vec3(delta_mouse_x, delta_mouse_y, 0.0f));
        delta_mouse_x = 0;
        delta_mouse_y = 0;
    }
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            break;
        }
        case WM_SIZE:
        {
            RECT client_area;
            GetClientRect(hWnd, &client_area);
            client_width = client_area.right - client_area.left;
            client_height = client_area.bottom - client_area.top;
            break;
        }
        case WM_KEYDOWN:
        {
            // wParam - key code
            keys_log[wParam] = true;
            break;
        }
        case WM_KEYUP:
        {
            keys_log[wParam] = false;
            break;
        }
        case WM_RBUTTONDOWN:
        {
            keys_log[KEY_RMOUSE] = true;
            break;
        }
        case WM_RBUTTONUP:
        {
            keys_log[KEY_RMOUSE] = false;
            break;
        }
        case WM_MOUSEMOVE:
        {
            calc_mouse_movement(lParam);
            break;
        }
        break;
    }

    // Handle all messages which we didn't handle above
    return DefWindowProc (hWnd, message, wParam, lParam);
}
