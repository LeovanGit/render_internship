#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include <vector>
#include "glm.hpp"

#include "window.hpp"
#include "scene.hpp"
#include "camera.hpp"
#include "sphere.hpp"

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
constexpr float ROTATIONAL_SPEED = 30.0f;

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
                      float delta_time,
                      Window & win);

    void calcMouseMovement(LPARAM lParam);

    Scene * scene;

    glm::vec3 mouse;

    glm::vec3 delta_mouse;

    glm::vec3 fixed_mouse;
    glm::vec3 delta_fixed_mouse;
    
    bool keys_log[KEYS_COUNT];
};

#endif
