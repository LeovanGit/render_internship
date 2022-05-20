#include <windows.h>
#include <windowsx.h>
#include <chrono>
#include <vector>
#include "glm.hpp"

#include "window.hpp"
#include "scene.hpp"
#include "controller.hpp"
#include "matrices.hpp"
#include "material.hpp"
#include "camera.hpp"
#include "sphere.hpp"

#define WIN_POS_X 300
#define WIN_POS_Y 300

#define CLIENT_WIDTH 384
#define CLIENT_HEIGHT 216

constexpr float FRAME_DURATION = 1.0f / 60.0f;

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
    win.init(hInstance,
             WIN_POS_X, WIN_POS_Y,
             CLIENT_WIDTH, CLIENT_HEIGHT);

    ShowWindow(win.handle, nCmdShow);

    // CREATE SCENE
    controller.init(&scene);

    // CREATE MATERIALS
    enum Materials
    {
        GLOSSY_BLUE,
        MATTE_GREY,
        MATTE_RED,
        MATTE_WHITE,
        HALF_GLOSSY_GREEN,
        GLOSSY_RED,
    };

    std::vector<Material> materials;
    // glossy blue
    materials.push_back(Material(glm::vec3(0.4f, 0.0f, 0.0f),
                                 1.0f,
                                 128.0f,
                                 glm::vec3(0.0f, 0.0f, 0.0f)));
    // matte grey
    materials.push_back(Material(glm::vec3(0.5f, 0.5f, 0.5f),
                                 0,
                                 0,
                                 glm::vec3(0, 0, 0)));
    // matte red
    materials.push_back(Material(glm::vec3(0.0f, 0.0f, 0.8f),
                                 0.2f,
                                 16.0f,
                                 glm::vec3(0.0f, 0.0f, 0.0f)));
    // matte white
    materials.push_back(Material(glm::vec3(1.0f, 1.0f, 1.0f),
                                 0.2f,
                                 16.0f,
                                 glm::vec3(0.0f, 0.0f, 0.0f)));

    // half-glossy green
    materials.push_back(Material(glm::vec3(0.0f, 0.4f, 0.0f),
                                 0.6f,
                                 72.0f,
                                 glm::vec3(0.0f, 0.0f, 0.0f)));
    // glossy red
    materials.push_back(Material(glm::vec3(0.0f, 0.0f, 0.8f),
                                 1.0f,
                                 128.0f,
                                 glm::vec3(0.0f, 0.0f, 0.0f)));

    // CREATE PLANES
    std::vector<Scene::Plane> planes;
    // floor
    planes.push_back(Scene::Plane(glm::vec3(0, 1.0f, 0),
                                  glm::vec3(0, -100.0f, 0),
                                  materials[MATTE_GREY]));


    // CREATE SPHERES
    std::vector<Scene::Sphere> spheres;
    spheres.push_back(Scene::Sphere(80.0f,
                                    glm::vec3(0, -20.0f, 0),
                                    materials[GLOSSY_BLUE]));

    spheres.push_back(Scene::Sphere(100.0f,
                                    glm::vec3(-180.0f, 200.0f, 100.0f),
                                    materials[MATTE_RED]));

    spheres.push_back(Scene::Sphere(120.0f,
                                    glm::vec3(300.0f, 20.0f, 100.0f),
                                    materials[HALF_GLOSSY_GREEN]));

    std::vector <Scene::Cube> cubes;
    cubes.push_back(Scene::Cube(glm::vec3(-180.0f, 0, 100.0f),
                                glm::vec3(0.0f, 45.0f, 0.0f),
                                glm::vec3(100.0f, 100.0f, 100.0f),
                                materials[MATTE_WHITE]));

    cubes.push_back(Scene::Cube(glm::vec3(-350.0f, -25.0f, -70.0f),
                                glm::vec3(0.0f, 45.0f, 0.0f),
                                glm::vec3(75.0f, 75.0f, 75.0f),
                                materials[GLOSSY_RED]));

    std::vector<Scene::PointLight> p_lights;
    p_lights.push_back(Scene::PointLight(glm::vec3(-400.0, 250.0f, -300.0f),
                                         350.0f,
                                         glm::vec3(0.8f)));
    
    std::vector<Scene::DirectionalLight> d_lights;
    d_lights.push_back(Scene::DirectionalLight(
                           glm::normalize(glm::vec3(1.0f, -1.0f, 1.0f)),
                           glm::vec3(0.3f)));

    std::vector<Scene::SpotLight> s_lights;
    s_lights.push_back(Scene::SpotLight(
                           glm::vec3(200, 200, -300),
                           250.0f,
                           55.0f,
                           glm::normalize(glm::vec3(0, -1.0f, 1.0f)),
                           glm::vec3(0.1f, 0.5f, 1.0f)));
                                        
    controller.initScene(spheres,
                         planes,
                         cubes,
                         d_lights,
                         p_lights,
                         s_lights);

    camera.setPerspective(45.0f,
                          float(CLIENT_WIDTH) / CLIENT_HEIGHT,
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
                                  (float)LOWORD(lParam) / HIWORD(lParam),
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
        break;
    }

    // Handle all messages which we didn't handle above
    return DefWindowProc (hWnd, message, wParam, lParam);
}
