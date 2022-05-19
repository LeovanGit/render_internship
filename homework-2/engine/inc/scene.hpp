#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>
#include <cmath>
#include <iostream>

#include "glm.hpp"
#include "matrices.hpp"

#include "window.hpp"
#include "colored_sphere.hpp"
#include "material.hpp"
#include "light.hpp"
#include "intersection.hpp"
#include "plane.hpp"
#include "camera.hpp"
#include "colored_plane.hpp"
#include "triangle.hpp"
#include "colored_triangle.hpp"
#include "cube.hpp"


constexpr int HEX_BLACK = 0x000000;
constexpr float ambient = 0.1f;
constexpr float delta = 0.001f;

class Scene
{
public:
    explicit Scene() = default;

    Scene(std::vector<ColoredSphere> c_spheres,
          std::vector<ColoredPlane> c_planes,
          std::vector<Cube> c_cubes,
          std::vector<PointLight> p_lights,
          std::vector<DirectionalLight> d_lights,
          std::vector<SpotLight> s_lights);

    bool findIntersection(Intersection & nearest, Ray & ray);

    bool isVisible(Intersection & nearest, glm::vec3 & dir_to_light);

    glm::vec3 blinnPhong(Intersection & nearest,
                         Camera & camera);

    void render(Window & win, Camera & camera);

    std::vector<ColoredSphere> c_spheres;
    std::vector<ColoredPlane> c_planes;
    std::vector<Cube> c_cubes;

    std::vector<PointLight> p_lights;
    std::vector<DirectionalLight> d_lights;
    std::vector<SpotLight> s_lights;
};

#endif
