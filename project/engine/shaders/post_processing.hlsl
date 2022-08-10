#include "globals.hlsl"

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

Texture2D g_HDR_scene;

//==============================================================================
// VERTEX SHADER
//==============================================================================
PS_INPUT vertexShader(uint vertex_index: SV_VERTEXID)
{
    // generate fullscreen triangle in clip space
    float2 vertexCS[3] = {{-1.0f, -1.0f},
                          {-1.0f,  3.0f},
                          { 3.0f, -1.0f}};
    
    PS_INPUT output;
    output.pos = float4(vertexCS[vertex_index], 1.0f, 1.0f);
    // inverse y
    output.uv = (vertexCS[vertex_index] + float2(1.0f, 1.0f)) / float2(2.0f, -2.0f);
    
    return output;
}

//==============================================================================
// FRAGMENT SHADER
//==============================================================================
float4 fragmentShader(PS_INPUT input) : SV_TARGET
{
    float4 color = g_HDR_scene.Sample(g_sampler, input.uv);
    return color;
}
