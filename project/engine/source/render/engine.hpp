#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "globals.hpp"
#include "shader_manager.hpp"
#include "texture_manager.hpp"
#include "model_manager.hpp"
#include "mesh_system.hpp"
#include "light_system.hpp"
#include "transform_system.hpp"
#include "particle_system.hpp"
#include "window.hpp"
#include "time_system.hpp"

namespace engine
{
class Engine
{
public:
    // deleted methods should be public for better error messages
    Engine() = delete;
    Engine(const Engine & other) = delete;
    void operator=(const Engine & other) = delete;

    static void init();
    
    static void del();
};
} // namespace engine

#endif
