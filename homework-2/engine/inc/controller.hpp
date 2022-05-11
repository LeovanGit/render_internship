#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include "window.hpp"
#include "scene.hpp"
#include <vector>

constexpr int KEYS_COUNT = 254; // 254 keys defined in WinAPI
constexpr int KEY_W = 87;
constexpr int KEY_A = 65;
constexpr int KEY_S = 83;
constexpr int KEY_D = 68;
constexpr int KEY_RMOUSE = 2;

class Controller
{
public:
    Controller() = default;

    void init(const Window & win, Scene * scene);

    void initScene(std::vector<ColoredSphere> & c_spheres,
                   std::vector<PointLight> & p_lights);

    //void processInput();

    void calcMouseMovement(LPARAM lParam);

    Scene * scene;

    int mouse_x;
    int mouse_y;

    int delta_mouse_x;
    int delta_mouse_y;
    
    bool keys_log[KEYS_COUNT];
};

#endif
