#ifndef SHADER_MANAGER_HPP
#define SHADER_MANAGER_HPP

#include <vector>

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

    void registerShader(const Shader & shader);

    void registerShader(WCHAR * filename,
                        D3D11_INPUT_ELEMENT_DESC input_desc[]);

    void useShader(int index);
       
private:
    ShaderManager() = default;
    ~ShaderManager() = default;

    static ShaderManager * instance;

    std::vector<Shader> shaders;
};
} // namespace engine

#endif
