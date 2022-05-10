#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>

#include "window.hpp"
#include "colored_sphere.hpp"
#include "material.hpp"
#include "light.hpp"
#include "intersection.hpp"

constexpr int HEX_BLACK = 0x000000;

class Scene
{
public:
    explicit Scene() = default;

    Scene(std::vector<ColoredSphere> c_spheres,
          std::vector<Light> lights);

    bool findIntersection(Intersection & nearest, Ray & ray);

    void render(Window & win);

    std::vector<ColoredSphere> c_spheres;
    std::vector<Light> lights;
};

#endif
