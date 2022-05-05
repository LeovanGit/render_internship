#include <windows.h>
#include <windowsx.h>

#include "engine.hpp"
#include "math.hpp"
#include "sphere.hpp"

#include <iostream>

#define WIN_POS_X 300
#define WIN_POS_Y 300

#define CLIENT_WIDTH 1000
#define CLIENT_HEIGHT 600

#define FPS 60

LRESULT CALLBACK WindowProc(HWND hWnd,
                            UINT message,
                            WPARAM wParam,
                            LPARAM lParam);

Window win;

Scene scene;

Controller controller;

float prev_time = GetTickCount();
float frame_time = 0;

bool frameTimeElapsed()
{
    float current_time = GetTickCount();
    frame_time += (current_time - prev_time);
    prev_time = current_time;

    if (frame_time >= (float)1000/FPS) // in ms
    {
        frame_time = 0;
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
    win = Window(hInstance,
                 WIN_POS_X, WIN_POS_Y,
                 CLIENT_WIDTH, CLIENT_HEIGHT);

    ShowWindow(win.handle, nCmdShow);

    scene = Scene();

    controller = Controller(win, &scene);

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
            controller.processInput();
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
