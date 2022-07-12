#include "win_def.hpp"
#include <d3d11.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <chrono>
#include <vector>
#include <string>
#include "glm.hpp"

#include <windows.h>
#include <windowsx.h>

// #include "scene.hpp"
// #include "matrices.hpp"
// #include "material.hpp"
// #include "sphere.hpp"
// #include "euler_angles.hpp"
#include "window.hpp"
#include "globals.hpp"
#include "shader.hpp"
#include "texture_manager.hpp"
#include "shader_manager.hpp"
#include "camera.hpp"
#include "controller.hpp"

#include "win_undef.hpp"

namespace
{
constexpr float FRAME_DURATION = 1.0f / 60.0f;

engine::windows::Window win;
// Scene scene;
Controller controller;

// CREATE CAMERA
Camera camera(glm::vec3(0, 0, -15.0f),
              glm::vec3(0, 1.0f, 0), // up
              glm::vec3(0, 0, 1.0f)); // forward
} // namespace

LRESULT CALLBACK WindowProc(HWND hWnd,
                            UINT message,
                            WPARAM wParam,
                            LPARAM lParam);

auto start_time = std::chrono::steady_clock::now();
float delta_time = 0;

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
    // INIT SINGLETONS
    engine::Globals::init();
    engine::ShaderManager::init(); // ShaderManager depends on Globals
    engine::TextureManager::init(); // TextureManager depends on Globals

    engine::Globals * globals = engine::Globals::getInstance();
    engine::ShaderManager * shader_mgr = engine::ShaderManager::getInstance();
    engine::TextureManager * tex_mgr = engine::TextureManager::getInstance();

    globals->initD3D();
    globals->initVBO();

    // SHADERS
    D3D11_INPUT_ELEMENT_DESC ied[] =
    {
        {"POSITION",
         0,
         DXGI_FORMAT_R32G32B32_FLOAT,
         0,
         D3D11_APPEND_ALIGNED_ELEMENT, // offset
         D3D11_INPUT_PER_VERTEX_DATA,
         0},

        {"TEXCOORD",
         0,
         DXGI_FORMAT_R32G32_FLOAT,
         0,
         D3D11_APPEND_ALIGNED_ELEMENT,
         D3D11_INPUT_PER_VERTEX_DATA,
         0},
    };
    shader_mgr->registerShader(L"../engine/shaders/cube.hlsl", ied);
    shader_mgr->registerShader(L"../engine/shaders/skybox.hlsl", ied);
    shader_mgr->registerShader(L"../engine/shaders/floor.hlsl", ied);
    
    // TEXTURES
    tex_mgr->registerTexture(L"../textures/rubik_cube.dds");
    tex_mgr->registerTexture(L"../textures/skybox.dds");
    tex_mgr->registerTexture(L"../textures/prototype_grid.dds");

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
    win.init(hInstance, 100.0f, 100.0f, 1280.0f, 720.0f);

    // CREATE SCENE
    // controller.init(&scene);                                        
    // controller.initScene();

    camera.setPerspective(glm::radians(45.0f),
                          1280.0f / 720.0f,
                          1.0f,
                          1000.0f);
    camera.updateMatrices();

    globals->setConstBuffer(camera);

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
            int fps = 1 / delta_time;
            std::string fps_str = "FPS: " + std::to_string(fps);
            SetWindowTextA(win.handle, TEXT(fps_str.c_str()));

            controller.processInput(camera, delta_time, win);
            camera.updateMatrices();
            globals->setConstBuffer(camera);
            // controller.scene->render(win, camera);            
            win.renderFrame();
        }
    }
    exit:
    // no need to clean COM objects,
    // because DxResPtr does it in the destructor!

    // DELETE SINGLETONS (in reverse order!!!)
    engine::TextureManager::del();
    engine::ShaderManager::del();
    engine::Globals::del();

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
        case WM_MOVE:
        {
            win.pos_x = LOWORD(lParam);
            win.pos_y = HIWORD(lParam);
            break;
        }
        case WM_SIZE:
        {
            win.resize(LOWORD(lParam), HIWORD(lParam));
            
            camera.setPerspective(glm::radians(45.0f),
                                  float(LOWORD(lParam)) / HIWORD(lParam),
                                  1.0f,
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
            controller.was_released[wParam] = true;
            break;
        }
        case WM_RBUTTONDOWN:
        {
            // controller.keys_log[KEY_RMOUSE] = true;
            break;
        }
        case WM_RBUTTONUP:
        {
            // controller.keys_log[KEY_RMOUSE] = false;
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
