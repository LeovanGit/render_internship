#include "shader.hpp"

namespace engine
{
Shader::Shader(const std::string & shader_path,
               D3D11_INPUT_ELEMENT_DESC input_desc[],
               uint32_t input_desc_size,
               bool vertex_shader,
               bool geometry_shader,
               bool fragment_shader)
{
    if (!vertex_shader && !geometry_shader && !fragment_shader) return;
    
    HRESULT result;
    Globals * globals = Globals::getInstance();

    ID3D10Blob * error_message(0);
    ID3D10Blob * vert_shader_buffer(0);
    ID3D10Blob * frag_shader_buffer(0);
    ID3D10Blob * geom_shader_buffer(0);

    // pass only path to shader
    ShaderIncluder includer(shader_path.substr(0, shader_path.rfind("/")));
    
    // create full path to shader
    std::wstring path = std::wstring(shader_path.begin(),
                                     shader_path.end());

    if (vertex_shader)
    {
        result = D3DCompileFromFile(path.c_str(),
                                    NULL,
                                    &includer,
                                    "vertexShader",
                                    "vs_5_0",
                                    SKIP_SHADER_OPTIMIZATIONS,
                                    0,
                                    &vert_shader_buffer,
                                    &error_message);

        if(FAILED(result))
        {
            if (error_message) std::cout << "\nVERTEX SHADER COMPILATION ERROR:\n"
                                         << (char *)error_message->GetBufferPointer()
                                         << "\n\n";
            else std::cout << "\nVERTEX SHADER NOT FOUND\n";
            assert(result >= 0 && "D3DCompileFromFile vertex shader");
        }

        result = globals->device5->
            CreateVertexShader(vert_shader_buffer->GetBufferPointer(),
                               vert_shader_buffer->GetBufferSize(),
                               NULL,
                               vert_shader.reset());
        assert(result >= 0 && "CreateVertexShader");

        if (input_desc)
        {
            globals->device5->
            CreateInputLayout(input_desc,
                              input_desc_size,
                              vert_shader_buffer->GetBufferPointer(),
                              vert_shader_buffer->GetBufferSize(),
                              input_layout.reset());        
        }
    }

    if (fragment_shader)
    {
        result = D3DCompileFromFile(path.c_str(),
                                    NULL,
                                    &includer,
                                    "fragmentShader",
                                    "ps_5_0",
                                    SKIP_SHADER_OPTIMIZATIONS,
                                    0,
                                    &frag_shader_buffer,
                                    &error_message);

        if(FAILED(result))
        {
            if (error_message) std::cout << "\nFRAGMENT SHADER COMPILATION ERROR:\n" 
                                         << (char *)error_message->GetBufferPointer()
                                         << "\n\n";
            else std::cout << "FRAGMENT SHADER NOT FOUND\n";
            assert(result >= 0 && "D3DCompileFromFile fragment shader");
        }

        result = globals->device5->
            CreatePixelShader(frag_shader_buffer->GetBufferPointer(),
                              frag_shader_buffer->GetBufferSize(),
                              NULL,
                              frag_shader.reset());
        assert(result >= 0 && "CreatePixelShader");
    }

    if (geometry_shader)
    {
        result = D3DCompileFromFile(path.c_str(),
                                    NULL,
                                    &includer,
                                    "fgeometryShader",
                                    "ps_5_0",
                                    SKIP_SHADER_OPTIMIZATIONS,
                                    0,
                                    &geom_shader_buffer,
                                    &error_message);

        if(FAILED(result))
        {
            if (error_message) std::cout << "\nGEOMETRY SHADER COMPILATION ERROR:\n" 
                                         << (char *)error_message->GetBufferPointer()
                                         << "\n\n";
            else std::cout << "GEOMETRY SHADER NOT FOUND\n";
            assert(result >= 0 && "D3DCompileFromFile geometry shader");
        }

        result = globals->device5->
            CreateGeometryShader(geom_shader_buffer->GetBufferPointer(),
                                 geom_shader_buffer->GetBufferSize(),
                                 NULL,
                                 geom_shader.reset());
        assert(result >= 0 && "CreateGeometryShader");
    }
}

void Shader::bind()
{
    Globals * globals = Globals::getInstance();

    // bind shaders and input layout
    globals->device_context4->VSSetShader(vert_shader.ptr(), 0, 0);
    globals->device_context4->PSSetShader(frag_shader.ptr(), 0, 0);
    globals->device_context4->IASetInputLayout(input_layout);
}
} // namespace engine
