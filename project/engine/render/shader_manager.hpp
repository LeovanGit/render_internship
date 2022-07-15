#ifndef SHADER_MANAGER_HPP
#define SHADER_MANAGER_HPP

#include <unordered_map>
#include <string>
#include "spdlog.h"

#include "globals.hpp"
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

    void registerShader(const std::string & key,
                        const Shader & shader);

    void registerShader(const std::string & key,
                        WCHAR * path,
                        WCHAR * filename,
                        D3D11_INPUT_ELEMENT_DESC input_desc[] = nullptr);

    void useShader(const std::string & key);
       
private:
    ShaderManager() = default;
    ~ShaderManager() = default;

    static ShaderManager * instance;

    std::unordered_map<std::string, Shader> shaders;
};
} // namespace engine

#endif
