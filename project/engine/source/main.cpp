#include "win_def.hpp"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <windowsx.h>
#include <string>
#include "glm.hpp"

#include "window.hpp"
#include "camera.hpp"
#include "post_process.hpp"
#include "controller.hpp"
#include "scene.hpp"
#include "engine.hpp"
#include "timer.hpp"
#include "additional.hpp"

#include "win_undef.hpp"

namespace
{
constexpr float FRAME_DURATION = 1.0f / 60.0f;

constexpr uint32_t WINDOW_INIT_POS_X = 100;
constexpr uint32_t WINDOW_INIT_POS_Y = 100;
constexpr uint32_t WINDOW_INIT_POS_WIDTH = 1280;
constexpr uint32_t WINDOW_INIT_POS_HEIGHT = 720;

engine::windows::Window win;
engine::Scene scene;
Controller controller;

// CREATE CAMERA
Camera camera(glm::vec3(0.0f, 0.0f, -30.0f),
              glm::vec3(0, 1.0f, 0), // up
              glm::vec3(0, 0, 1.0f)); // forward

engine::Postprocess post_process;
} // namespace

LRESULT CALLBACK WindowProc(HWND hWnd,
                            UINT message,
                            WPARAM wParam,
                            LPARAM lParam);

engine::Timer timer;
float delta_time = 0.0f;

bool frameTimeElapsed()
{
    float elapsed_time = timer.getElapsedTime();
    
    if (elapsed_time >= FRAME_DURATION)
    {
        timer.restart();
        delta_time = elapsed_time;
        
        return true;
    }
    return false;
}

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    // INIT ENGINE
    engine::Engine::init();
    
    // REGISTER WINDOW CLASS
    WNDCLASSEX wclass;
    ZeroMemory(&wclass, sizeof(WNDCLASSEX));

    wclass.cbSize = sizeof(WNDCLASSEX);
    wclass.style = CS_HREDRAW | CS_VREDRAW;
    wclass.lpfnWndProc = WindowProc;
    wclass.hInstance = hInstance;
    wclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wclass.lpszClassName = "window_class_1";

    RegisterClassEx(&wclass);

    // CREATE WINDOW
    win.init(hInstance,
             WINDOW_INIT_POS_X,
             WINDOW_INIT_POS_Y,
             WINDOW_INIT_POS_WIDTH,
             WINDOW_INIT_POS_HEIGHT);

    // CREATE SCENE
    scene.init(win);
    controller.init(scene, post_process);
    controller.initScene(camera);
    controller.initPostprocess();
    
    ShowWindow(win.handle, nCmdShow);

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
            int fps = static_cast<int>(1.0f / delta_time);
            std::string fps_str = "FPS: " + std::to_string(fps);
            SetWindowTextA(win.handle, TEXT(fps_str.c_str()));

            controller.processInput(camera, post_process, delta_time, win);
            camera.updateMatrices();
            engine::moveDissolutionToOpaqueInstances();
            controller.scene->renderFrame(win, camera, post_process, delta_time);
        }
    }
    exit:
    // no need to clean COM objects,
    // because DxResPtr does it in the destructor!

    // CLEAN ENGINE
    engine::Engine::del();

    // return this part of the WM_QUIT message to Windows
    return static_cast<int>(msg.wParam);
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
        case WM_MOVE:
        {
            win.pos_x = LOWORD(lParam);
            win.pos_y = HIWORD(lParam);
            break;
        }
        case WM_SIZE:
        {
            if (wParam != SIZE_MINIMIZED)
            {
                win.resize(LOWORD(lParam), HIWORD(lParam));
                scene.initDepthBuffer(LOWORD(lParam), HIWORD(lParam));
                scene.initRenderTarget(LOWORD(lParam), HIWORD(lParam));

                camera.setPerspective(glm::radians(45.0f),
                                      float(LOWORD(lParam)) / HIWORD(lParam),
                                      1.0f,
                                      1000.0f);
            }

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
            controller.was_released[wParam] = true;
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
            controller.mouse.x = GET_X_LPARAM(lParam);
            controller.mouse.y = GET_Y_LPARAM(lParam);
            break;
        }
	case WM_MOUSEWHEEL:
        {
            float wheel_delta = GET_WHEEL_DELTA_WPARAM(wParam);
            if (wheel_delta >= 0) controller.camera_movement_speed *= 1.1f;
            else controller.camera_movement_speed /= 1.1f;
            break;
        }
        break;
    }

    // Handle all messages which we didn't handle above
    return DefWindowProc (hWnd, message, wParam, lParam);
}
