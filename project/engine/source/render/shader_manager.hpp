#ifndef SHADER_MANAGER_HPP
#define SHADER_MANAGER_HPP

#include <unordered_map>
#include <string>
#include "spdlog.h"
#include <memory>

#include "dx_res_ptr.hpp"
#include "shader.hpp"

namespace engine
{
// Singleton for shaders resources
class ShaderManager final
{
public:
    // deleted methods should be public for better error messages
    ShaderManager(const ShaderManager & other) = delete;
    void operator=(const ShaderManager & other) = delete;

    static void init();

    static ShaderManager * getInstance();

    static void del();

    std::shared_ptr<Shader> getShader(const std::string shader_path,
                                      D3D11_INPUT_ELEMENT_DESC input_desc[] = nullptr,
                                      uint32_t input_desc_size = 0,
                                      bool vertex_shader = true,
                                      bool geometry_shader = false,
                                      bool fragment_shader = true);

    void bindShader(const std::string & key);
       
private:
    ShaderManager() = default;
    ~ShaderManager() = default;

    static ShaderManager * instance;

    std::unordered_map<std::string, std::shared_ptr<Shader>> shaders;
};
} // namespace engine

#endif
