#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "sphere.hpp"
#include "window.hpp"

constexpr int HEX_WHITE = 0xFFFFFF;
constexpr int HEX_BLACK = 0x000000;

class Scene
{
public:
    Scene(Sphere sphere = Sphere());

    void render(Window & win);

    Sphere sphere;
};

#endif
