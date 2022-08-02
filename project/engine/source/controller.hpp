#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include "glm.hpp"

#include "window.hpp"
#include "camera.hpp"
#include "euler_angles.hpp"
#include "matrices.hpp"
#include "scene.hpp"
#include "sky.hpp"
#include "globals.hpp"
#include "shader_manager.hpp"
#include "texture_manager.hpp"
#include "model_manager.hpp"
#include "mesh_system.hpp"
#include "opaque_instances.hpp"

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

typedef engine::OpaqueInstances oi;

class Controller
{
public:
    Controller() = default;

    void init(engine::Scene & scene);

    void initScene(Camera & camera);

    void processInput(Camera & camera,
                      const float delta_time,
                      const engine::windows::Window & win);

    void calcMouseMovement(LPARAM lParam);

    engine::Scene * scene;

    glm::vec2 mouse;
    glm::vec2 fixed_mouse;
    glm::vec2 delta_fixed_mouse;   

    bool keys_log[KEYS_COUNT];
    bool was_released[KEYS_COUNT];

    glm::vec3 movement_speed = glm::vec3(5.0f);
    glm::vec3 rotation_speed = glm::vec3(360.0f, 360.0f, 60.0f);

    bool is_accelerated;

private:
    void initKnight(const math::Transform & transform);

    void initCube(const std::string & texture_path,
                  const math::Transform & transform);
};

#endif
