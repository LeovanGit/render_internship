#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include "glm.hpp"

#include "window.hpp"
#include "camera.hpp"
#include "euler_angles.hpp"
#include "matrices.hpp"
#include "renderer.hpp"
#include "sky.hpp"
#include "globals.hpp"
#include "shader_manager.hpp"
#include "texture_manager.hpp"
#include "model_manager.hpp"
#include "mesh_system.hpp"
#include "light_system.hpp"
#include "transform_system.hpp"
#include "opaque_instances.hpp"
#include "emissive_instances.hpp"
#include "post_process.hpp"
#include "particle_system.hpp"
#include "engine.hpp"
#include "time_system.hpp"
#include "grass_system.hpp"
#include "decal_system.hpp"
#include "additional.hpp"

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
constexpr int KEY_T = 84;
constexpr int KEY_N = 78;
constexpr int KEY_M = 77;
constexpr int KEY_F = 70;
constexpr int KEY_SHIFT = 16;
constexpr int KEY_LMOUSE = 1;
constexpr int KEY_RMOUSE = 2;

typedef engine::OpaqueInstances oi;
typedef engine::EmissiveInstances ei;
typedef engine::DissolutionInstances di;

class Controller
{
public:
    Controller() = default;

    void init(engine::Renderer & renderer,
              engine::Postprocess & post_process);

    void initScene(Camera & camera);

    void initPostprocess();

    void moveDissolutionToOpaqueInstances();
    
    void processInput(Camera & camera,
                      engine::Postprocess & post_process, 
                      const float delta_time,
                      const engine::windows::Window & win);

    engine::Renderer * renderer;
    engine::Postprocess * post_process;

    glm::ivec2 mouse;
    glm::ivec2 fixed_mouse;

    bool keys_log[KEYS_COUNT];
    bool was_released[KEYS_COUNT];

    glm::vec3 camera_movement_speed = glm::vec3(5.0f);
    glm::vec3 camera_rotation_speed = glm::vec3(360.0f, 360.0f, 60.0f);

    math::EulerAngles object_rotation_speed = math::EulerAngles(10.0f, 0.0f, 0.0f);

    bool is_accelerated;

    struct GrabbedObject
    {
        bool is_grabbed = false;
        uint32_t transform_id;
        float t;
        glm::vec3 pos;
    } object;

private:
    void initKnight(const math::Transform & transform);
    void spawnKnight(const math::Transform & transform);

    void initWall(const math::Transform & transform);

    void initCube(const math::Transform & transform,
                  const std::vector<oi::Material> & materials);

    void initPlane(const math::Transform & transform,
                   const std::vector<oi::Material> & materials);

    void initSphere(const math::Transform & transform,
                    const std::vector<oi::Material> & materials);

    void initFloor(const std::vector<oi::Material> & materials);

    void initSwamp(const std::vector<oi::Material> & materials);
    
    void initDirectionalLight(const glm::vec3 & radiance,
                              const glm::vec3 & direction,
                              float solid_angle);

    void initPointLight(const glm::vec3 & position,
                        const glm::vec3 & irradiance,
                        float distance,
                        float radius);

    void initShaders();
    void initTextures();
    void initSceneObjects();
    void initParticleEmitters();
    void initGrassFields();
};

#endif
