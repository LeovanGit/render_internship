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
float4 adjustExposure(float4 color);

float4 fragmentShader(PS_INPUT input) : SV_TARGET
{
    float4 color = g_HDR_scene.Sample(g_sampler, input.uv);

    // color = adjustExposure(color);
    if (g_EV_100 > 0) color = float4(1.0f, 0.0f, 0.0f, 1.0f);
    else if (g_EV_100 < 0) color = float4(0.0f, 1.0f, 0.0f, 1.0f);
    else color = float4(0.0f, 0.0f, 1.0f, 1.0f);
    
    return color;
}

float4 adjustExposure(float4 color)
{
    float S = 100.0f;
    float q = 0.65f;

    float luminance_max = (78.0f / (q * S)) * pow(2.0f, g_EV_100);
    return color * (1.0f / luminance_max);
}
