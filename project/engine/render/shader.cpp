#include "shader.hpp"

namespace engine
{
void Shader::init(const engine::windows::Window & win,
                  WCHAR * vert_filename,
                  WCHAR * frag_filename)
{
    HRESULT result;
    ID3D10Blob * errorMessage(0);
    ID3D10Blob * vertexShaderBuffer(0);
    ID3D10Blob * pixelShaderBuffer(0);
    D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
    unsigned int numElements;
    D3D11_BUFFER_DESC matrixBufferDesc;

    // Compile the vertex shader code
    result = D3DCompileFromFile(vert_filename,
                                NULL,
                                NULL,
                                "vertexShader",
                                "vs_5_0",
                                D3D10_SHADER_ENABLE_STRICTNESS,
                                0,
                                &vertexShaderBuffer,
                                &errorMessage);

    if(FAILED(result))
    {
        if (errorMessage) std::cout << "\nVERTEX SHADER COMPILATION ERROR:\n"
                                    << (char *)errorMessage->GetBufferPointer()
                                    << "\n\n";
        else std::cout << "\nVERTEX SHADER NOT FOUND\n";
        assert(result >= 0);
    }

    // Compile the fragment shader code
    result = D3DCompileFromFile(frag_filename,
                                NULL,
                                NULL,
                                "fragmentShader",
                                "ps_5_0",
                                D3D10_SHADER_ENABLE_STRICTNESS,
                                0,
                                &pixelShaderBuffer,
                                &errorMessage);

    if(FAILED(result))
    {
        if (errorMessage) std::cout << "\nFRAGMENT SHADER COMPILATION ERROR:\n" 
                                    << (char *)errorMessage->GetBufferPointer()
                                    << "\n\n";
        else std::cout << "FRAGMENT SHADER NOT FOUND\n";
        assert(result >= 0);
    }

    // Create the vertex shader from the buffer
    result = s_device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(),
                                          vertexShaderBuffer->GetBufferSize(),
                                          NULL,
                                          vert_shader.reset());
    assert(result >= 0 && "CreateVertexShader");

    // Create the fragment shader from the buffer
    result = s_device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(),
                                         pixelShaderBuffer->GetBufferSize(),
                                         NULL,
                                         frag_shader.reset());
    assert(result >= 0 && "CreatePixelShader");

    // activate shaders
    s_device_context->VSSetShader(vert_shader.ptr(), 0, 0);
    s_device_context->PSSetShader(frag_shader.ptr(), 0, 0);

    // CREATE VAO (INPUT LAYOUT)
    D3D11_INPUT_ELEMENT_DESC ied[] =
    {
        // use D3D11_APPEND_ALIGNED_ELEMENT to auto calculate offset
        {"POSITION",
         0,
         DXGI_FORMAT_R32G32B32_FLOAT,
         0,
         0,
         D3D11_INPUT_PER_VERTEX_DATA,
         0},

        {"COLOR",
         0,
         DXGI_FORMAT_R32G32B32A32_FLOAT,
         0,
         12, // offset
         D3D11_INPUT_PER_VERTEX_DATA,
         0},
    };

    s_device->CreateInputLayout(ied,
                                2,
                                vertexShaderBuffer->GetBufferPointer(),
                                vertexShaderBuffer->GetBufferSize(),
                                &s_input_layout);

    s_device_context->IASetInputLayout(s_input_layout);
}
} // namespace engine
