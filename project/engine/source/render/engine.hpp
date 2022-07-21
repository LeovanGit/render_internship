#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "globals.hpp"
#include "shader_manager.hpp"
#include "texture_manager.hpp"

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
