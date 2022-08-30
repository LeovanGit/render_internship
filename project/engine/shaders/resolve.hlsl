#include "globals.hlsl"

struct PS_INPUT
{
    float4 pos : SV_POSITION; 
};

Texture2D g_hdr_scene;

//------------------------------------------------------------------------------
// VERTEX SHADER
//------------------------------------------------------------------------------
PS_INPUT vertexShader(uint vertex_index: SV_VERTEXID)
{
    // generate fullscreen triangle in clip space
    float2 vertexCS[3] = {{-1.0f, -1.0f},
                          {-1.0f,  3.0f},
                          { 3.0f, -1.0f}};
    
    PS_INPUT output;
    output.pos = float4(vertexCS[vertex_index], 1.0f, 1.0f);

    return output;
}

//------------------------------------------------------------------------------
// FRAGMENT SHADER
//------------------------------------------------------------------------------
float3 adjustExposure(float3 color)
{
    float S = 100.0f;
    float q = 0.65f;

    float luminance_max = (78.0f / (q * S)) * pow(2.0f, g_EV_100);
    return color * (1.0f / luminance_max);
}

float3 toneMappingACES(float3 hdr)
{
    float3x3 m1 = float3x3(0.59719f, 0.07600f, 0.02840f,
                           0.35458f, 0.90834f, 0.13383f,
                           0.04823f, 0.01566f, 0.83777f);

    float3x3 m2 = float3x3(1.60475f, -0.10208, -0.00327f,
                           -0.53108f,  1.10813, -0.07276f,
                           -0.07367f, -0.00605,  1.07602f);

    float3 v = mul(hdr, m1);
    float3 a = v * (v + 0.0245786f) - 0.000090537f;
    float3 b = v * (0.983729f * v + 0.4329510f) + 0.238081f;
    float3 ldr = clamp(mul((a / b), m2), 0.0f, 1.0f);

    return ldr;
}

float3 gammaCorrection(float3 color)
{
    return pow(color, 1.0f / g_gamma);
}

float4 fragmentShader(PS_INPUT input) : SV_TARGET
{
    float3 color = g_hdr_scene.Load(int3(input.pos.x, input.pos.y, 0));

    color = adjustExposure(color);
    color = toneMappingACES(color);
    color = gammaCorrection(color);
    
    return float4(color, 1.0f);
}

