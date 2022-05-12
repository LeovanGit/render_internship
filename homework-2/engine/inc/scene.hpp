#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>
#include <cmath>
#include <iostream>

#include "glm.hpp"

#include "window.hpp"
#include "colored_sphere.hpp"
#include "material.hpp"
#include "light.hpp"
#include "intersection.hpp"
#include "plane.hpp"

constexpr int HEX_BLACK = 0x000000;
constexpr float ambient = 0.1f;

class Scene
{
public:
    explicit Scene() = default;

    Scene(std::vector<ColoredSphere> c_spheres,
          std::vector<Plane> planes,
          std::vector<PointLight> p_lights);

    bool findIntersection(Intersection & nearest, Ray & ray);


    glm::vec3 blinnPhong(Intersection & nearest,
                         glm::vec3 camera,
                         bool visibility);

    void render(Window & win);

    std::vector<ColoredSphere> c_spheres;
    std::vector<Plane> planes;

    std::vector<PointLight> p_lights;
};

#endif
