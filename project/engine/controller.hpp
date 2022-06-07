#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include <vector>
#include "glm.hpp"

#include "window.hpp"
#include "scene.hpp"
#include "camera.hpp"
#include "sphere.hpp"
#include "plane.hpp"
#include "euler_angles.hpp"
#include "material.hpp"

constexpr int KEYS_COUNT = 254; // 254 keys defined in WinAPI

constexpr int KEY_W = 87;
constexpr int KEY_A = 65;
constexpr int KEY_S = 83;
constexpr int KEY_D = 68;
constexpr int KEY_CTRL = 17;
constexpr int KEY_SPACE = 32;
constexpr int KEY_Q = 81;
constexpr int KEY_E = 69;
constexpr int KEY_PLUS = 187;
constexpr int KEY_MINUS = 189;
constexpr int KEY_R = 82;
constexpr int KEY_G = 71;
constexpr int KEY_SHIFT = 16;

constexpr int KEY_LMOUSE = 1;
constexpr int KEY_RMOUSE = 2;

class Controller
{
public:
    Controller() = default;

    void init(Scene * scene);

    void initScene();

    void processInput(Camera & camera,
                      const float delta_time,
                      const Window & win);

    void calcMouseMovement(LPARAM lParam);

    Scene * scene;

    glm::vec2 mouse;
    glm::vec2 fixed_mouse;
    glm::vec2 delta_fixed_mouse;   

    bool keys_log[KEYS_COUNT];

    glm::vec3 movement_speed = glm::vec3(500.0f);
    glm::vec3 rotation_speed = glm::vec3(360.0f, 360.0f, 60.0f);

    bool is_accelerated = false;

    class GrabbedObject
    {
    public:
        GrabbedObject() = default;
        
        bool is_grabbed = false;
        float t;
        glm::vec3 point;
        std::unique_ptr<Scene::IObjectMover> mover;
    } object;
};

#endif
