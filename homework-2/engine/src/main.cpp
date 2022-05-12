#include <windows.h>
#include <windowsx.h>
#include <chrono>
#include <vector>

#include "glm.hpp"
#include "matrices.hpp"

#include "window.hpp"
#include "scene.hpp"
#include "controller.hpp"

#include "material.hpp"

#include "plane.hpp"
#include "colored_sphere.hpp"
#include "light.hpp"
#include "camera.hpp"

#define WIN_POS_X 300
#define WIN_POS_Y 300

#define CLIENT_WIDTH 1000
#define CLIENT_HEIGHT 600

constexpr float FRAME_DURATION = 1.0f / 60.0f;

LRESULT CALLBACK WindowProc(HWND hWnd,
                            UINT message,
                            WPARAM wParam,
                            LPARAM lParam);

Window win;
Scene scene;
Controller controller;

auto start_time = std::chrono::steady_clock::now();

bool frameTimeElapsed()
{
    using namespace std::chrono;

    duration<float> elapsed_time = steady_clock::now() - start_time;
        
    if (elapsed_time.count() >= FRAME_DURATION)
    {
        start_time = steady_clock::now();
        return true;
    }
    return false;
}

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    // REGISTER WINDOW CLASS
    WNDCLASSEX wclass;
    ZeroMemory(&wclass, sizeof(WNDCLASSEX));

    wclass.cbSize = sizeof(WNDCLASSEX);
    wclass.style = CS_HREDRAW | CS_VREDRAW;
    wclass.lpfnWndProc = WindowProc;
    wclass.hInstance = hInstance;
    wclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wclass.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wclass.lpszClassName = "window_class_1";

    RegisterClassEx(&wclass);

    // CREATE WINDOW
    win.init(hInstance,
             WIN_POS_X, WIN_POS_Y,
             CLIENT_WIDTH, CLIENT_HEIGHT);

    ShowWindow(win.handle, nCmdShow);

    // CREATE SCENE
    controller.init(win, &scene);

    std::vector<ColoredSphere> c_spheres;

    // blue
    c_spheres.push_back(ColoredSphere(Sphere(100,
                                             glm::vec3(-110.0f, 105.0f, 150.0f)),
                                      Material(glm::vec3(0.8f, 0.0f, 0.0f),
                                               1.0f,
                                               128.0f,
                                               glm::vec3(0.0f, 0.0f, 0.0f))));

    // red
    c_spheres.push_back(ColoredSphere(Sphere(100,
                                             glm::vec3(110.0f, 105.0f, 0)),
                                      Material(glm::vec3(0.0f, 0.0f, 0.8f),
                                               0.2f,
                                               16.0f,
                                               glm::vec3(0.0f, 0.0f, 0.0f))));

    // green
    c_spheres.push_back(ColoredSphere(Sphere(100,
                                             glm::vec3(0.0f, -100.0f, 300.0f)),
                                      Material(glm::vec3(0.0f, 0.8f, 0.0f),
                                               0.6f,
                                               72.0f,
                                               glm::vec3(0.0f, 0.0f, 0.0f))));

    std::vector<Plane> planes;
    // floor
    planes.push_back(Plane(glm::vec3(-10.0f, 107.0f, 0),
                           glm::vec3(0, 107.0f, 0),
                           glm::vec3(10.0f, 107.0f, 10.0f)));

    std::vector<PointLight> p_lights;
    p_lights.push_back(PointLight(glm::vec3(1.0f, 1.0f, 1.0f),
                                  250.0f,
                                  Sphere(10,
                                         glm::vec3(200, -200, -300)),
                                  Material(glm::vec3(1.0f, 1.0f, 1.0f),
                                           0.0f, 0.0f, 
                                           glm::vec3(0.0f, 0.0f, 0.0f))));

    Camera camera(glm::vec3(0, 0, -500.0f),
                  glm::vec3(0, -1.0f, 0),
                  glm::vec3(0, 0, 1.0f));

    controller.initScene(camera, c_spheres, planes, p_lights);

    // MAIN LOOP (EVENT HANDLING)
    MSG msg;

    while (true)
    {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            // quit
            if (msg.message == WM_QUIT) goto exit;
        }
                
        if (frameTimeElapsed()) 
        {
            //controller.processInput();
            controller.scene->render(win);
        }
    }
    exit:
    // return this part of the WM_QUIT message to Windows
    return msg.wParam;
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
            win.resize(LOWORD(lParam), HIWORD(lParam));
            break;
        }
        case WM_KEYDOWN:
        {
            // wParam - key code
            controller.keys_log[wParam] = true;
            break;
        }
        case WM_KEYUP:
        {
            controller.keys_log[wParam] = false;
            break;
        }
        case WM_RBUTTONDOWN:
        {
            controller.keys_log[KEY_RMOUSE] = true;
            break;
        }
        case WM_RBUTTONUP:
        {
            controller.keys_log[KEY_RMOUSE] = false;
            break;
        }
        case WM_MOUSEMOVE:
        {
            controller.calcMouseMovement(lParam);
            break;
        }
        break;
    }

    // Handle all messages which we didn't handle above
    return DefWindowProc (hWnd, message, wParam, lParam);
}
