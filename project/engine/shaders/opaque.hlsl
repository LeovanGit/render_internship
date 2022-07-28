#include "globals.hlsl"

struct VS_INPUT
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

Texture2D g_texture;

//------------------------------------------------------------------------------
// VERTEX SHADER
//------------------------------------------------------------------------------
PS_INPUT vertexShader(VS_INPUT input)
{
    float4x4 scale = float4x4(10.0f, 0.0f, 0.0f, 0.0f,
                              0.0f, 10.0f, 0.0f, 0.0f,
                              0.0f, 0.0f, 10.0f, 0.0f,
                              0.0f, 0.0f, 0.0f, 1.0f);

    
    PS_INPUT output;
    output.uv = input.uv;
    // output.pos = mul(mul(mul(float4(input.pos, 1.0f), scale), mesh_to_model), g_proj_view);
    output.pos = mul(mul(float4(input.pos, 1.0f), scale), g_proj_view);
    //output.pos = mul(g_proj_view, float4(input.pos, 1.0f));

    return output;
}

//------------------------------------------------------------------------------
// FRAGMENT SHADER
//------------------------------------------------------------------------------
float4 fragmentShader(PS_INPUT input) : SV_Target
{
    float3 color = g_texture.Sample(g_sampler, input.uv);
    // float3 color = g_texture.SampleLevel(g_sampler_state, input.uv, 0);

    return float4(color, 1.0f);
}
