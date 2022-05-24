#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include <vector>
#include "glm.hpp"

#include "window.hpp"
#include "scene.hpp"
#include "camera.hpp"
#include "sphere.hpp"

#include <iostream>

constexpr int KEYS_COUNT = 254; // 254 keys defined in WinAPI
constexpr int KEY_W = 87;
constexpr int KEY_A = 65;
constexpr int KEY_S = 83;
constexpr int KEY_D = 68;
constexpr int KEY_CTRL = 17;
constexpr int KEY_SPACE = 32;
constexpr int KEY_Q = 81;
constexpr int KEY_E = 69;
constexpr int KEY_LMOUSE = 1;
constexpr int KEY_RMOUSE = 2;

constexpr float MOVEMENT_SPEED = 500.0f;
constexpr float Z_ROTATIONAL_SPEED = 50.0f;

class Controller
{
public:
    Controller() = default;

    void init(Scene * scene);

    void initScene(const std::vector<Scene::Sphere> & spheres,
                   const std::vector<Scene::Plane> & planes,
                   const std::vector<Scene::Cube> & cubes,
                   const std::vector<Scene::DirectionalLight> & d_lights,
                   const std::vector<Scene::PointLight> & p_lights,
                   const std::vector<Scene::SpotLight> & s_lights);

    void processInput(Camera & camera,
                      const float delta_time,
                      const Window & win);

    void calcMouseMovement(LPARAM lParam);

    Scene * scene;

    glm::vec2 mouse;
    glm::vec2 fixed_mouse;
    glm::vec2 delta_fixed_mouse;   

    bool keys_log[KEYS_COUNT];

    class Object
    {
    public:
        Object() = default;
        
        bool is_grabbed = false;
        glm::vec3 grabbed_point;
        std::unique_ptr<Scene::IObjectMover> mover;
    } current_object;
};

#endif
