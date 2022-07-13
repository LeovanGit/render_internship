#include "shader.hpp"

namespace engine
{
Shader::Shader(WCHAR * path,
               WCHAR * filename,
               D3D11_INPUT_ELEMENT_DESC input_desc[])
{
    HRESULT result;

    ID3D10Blob * error_message(0);
    ID3D10Blob * vert_shader_buffer(0);
    ID3D10Blob * frag_shader_buffer(0);
    
    ShaderIncluder includer(path);
    
    // create full path to shader
    std::wstring shader_path = std::wstring(path);
    if (shader_path.back() == '/') shader_path += std::wstring(filename);
    else shader_path += L'/' + std::wstring(filename);

    // Compile the vertex shader code
    result = D3DCompileFromFile(shader_path.c_str(),
                                NULL,
                                &includer,
                                "vertexShader", // entry point
                                "vs_5_0",
                                // for graphics debugging tools like RenderDoc:
                                D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
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

    // Compile the fragment shader code
    result = D3DCompileFromFile(shader_path.c_str(),
                                NULL,
                                &includer,
                                "fragmentShader", // entry point
                                "ps_5_0",
                                D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
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

    // get access to global render variables
    Globals * globals = Globals::getInstance();

    // Create the vertex shader from the buffer
    result = globals->device5->
        CreateVertexShader(vert_shader_buffer->GetBufferPointer(),
                           vert_shader_buffer->GetBufferSize(),
                           NULL,
                           vert_shader.reset());
    assert(result >= 0 && "CreateVertexShader");

    // Create the fragment shader from the buffer
    result = globals->device5->
        CreatePixelShader(frag_shader_buffer->GetBufferPointer(),
                          frag_shader_buffer->GetBufferSize(),
                          NULL,
                          frag_shader.reset());
    assert(result >= 0 && "CreatePixelShader");

    // CREATE INPUT LAYOUT (VAO)
    if (input_desc)
        globals->device5->
            CreateInputLayout(input_desc,
                              2,
                              vert_shader_buffer->GetBufferPointer(),
                              vert_shader_buffer->GetBufferSize(),
                              input_layout.reset());
}

void Shader::activate()
{
    Globals * globals = Globals::getInstance();

    // bind shaders and input layout
    globals->device_context4->VSSetShader(vert_shader.ptr(), 0, 0);
    globals->device_context4->PSSetShader(frag_shader.ptr(), 0, 0);
    globals->device_context4->IASetInputLayout(input_layout);
}
} // namespace engine
