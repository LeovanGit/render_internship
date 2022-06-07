#include <windows.h>
#include <windowsx.h>
#include <chrono>
#include <vector>
#include "euler_angles.hpp"
#include "glm.hpp"
#include <string>

#include "window.hpp"
#include "scene.hpp"
#include "controller.hpp"
#include "matrices.hpp"
#include "material.hpp"
#include "camera.hpp"
#include "sphere.hpp"

namespace
{
constexpr float FRAME_DURATION = 1.0f / 60.0f;
} // namespace

LRESULT CALLBACK WindowProc(HWND hWnd,
                            UINT message,
                            WPARAM wParam,
                            LPARAM lParam);

Window win;
Scene scene;
Controller controller;

auto start_time = std::chrono::steady_clock::now();
float delta_time = 0;

// CREATE CAMERA
Camera camera(glm::vec3(0, 0, -1000.0f),
              glm::vec3(0, 1.0f, 0), // up
              glm::vec3(0, 0, 1.0f)); // forward

bool frameTimeElapsed()
{
    using namespace std::chrono;

    duration<float> elapsed_time = steady_clock::now() - start_time;
        
    if (elapsed_time.count() >= FRAME_DURATION)
    {
        delta_time = elapsed_time.count();

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
    win.init(hInstance, 300.0f, 300.0f, 384.0f, 216.0f);

    ShowWindow(win.handle, nCmdShow);

    // CREATE SCENE
    controller.init(&scene);                                        
    controller.initScene();

    camera.setPerspective(45.0f,
                          384.0f / 216.0f,
                          10.0f,
                          1000.0f);

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
            int fps = 1 / delta_time;
            std::string fps_str = "FPS: " + std::to_string(fps);
            SetWindowTextA(win.handle, TEXT(fps_str.c_str()));

            controller.processInput(camera, delta_time, win);
            camera.updateMatrices();
            controller.scene->render(win, camera);
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
            camera.setPerspective(45.0f,
                                  float(LOWORD(lParam)) / HIWORD(lParam),
                                  10.0f,
                                  1000.0f);
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
        case WM_LBUTTONDOWN:
        {
            // fix mouse
            controller.fixed_mouse.x = controller.mouse.x;
            controller.fixed_mouse.y = controller.mouse.y;

            controller.keys_log[KEY_LMOUSE] = true;
            break;
        }
        case WM_LBUTTONUP:
        {
            controller.keys_log[KEY_LMOUSE] = false;
            break;
        }
        case WM_MOUSEMOVE:
        {
            controller.calcMouseMovement(lParam);
            break;
        }
	case WM_MOUSEWHEEL:
        {
            float wheel_delta = GET_WHEEL_DELTA_WPARAM(wParam);
            if (wheel_delta >= 0) controller.movement_speed *= 1.1f;
            else controller.movement_speed /= 1.1f;
            break;
        }
        break;
    }

    // Handle all messages which we didn't handle above
    return DefWindowProc (hWnd, message, wParam, lParam);
}
