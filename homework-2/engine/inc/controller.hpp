#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include "window.hpp"
#include "scene.hpp"
#include "sphere.hpp"
#include "light.hpp"
#include "plane.hpp"
#include "camera.hpp"
#include "glm.hpp"
#include "colored_plane.hpp"
#include "triangle.hpp"
#include "colored_triangle.hpp"
#include "cube.hpp"

#include <vector>

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

    void init(const Window & win, Scene * scene);

    void initScene(std::vector<ColoredSphere> & c_spheres,
                   std::vector<ColoredPlane> & c_planes,
                   std::vector<Cube> & c_cubes,
                   std::vector<PointLight> & p_lights,
                   std::vector<DirectionalLight> & d_lights,
                   std::vector<SpotLight> & s_lights);

    void processInput(Camera & camera, float delta_time);

    void calcMouseMovement(LPARAM lParam);

    Scene * scene;

    glm::vec3 mouse;
    glm::vec3 fixed_mouse;
    glm::vec3 delta_mouse;
    
    bool keys_log[KEYS_COUNT];
};

#endif
