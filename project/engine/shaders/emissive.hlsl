#include "globals.hlsl"

cbuffer PerEmissiveMesh : register(b2)
{
    row_major float4x4 g_mesh_to_model;
    
    float3 g_radiance;
    float padding_0;
}

struct VS_INPUT
{
    float3 pos : POSITION;
    
    float4 transform_0 : TRANSFORM0;
    float4 transform_1 : TRANSFORM1;
    float4 transform_2 : TRANSFORM2;
    float4 transform_3 : TRANSFORM3;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
};

//------------------------------------------------------------------------------
// VERTEX SHADER
//------------------------------------------------------------------------------
PS_INPUT vertexShader(VS_INPUT input)
{
    float4x4 transform = float4x4(input.transform_0,
                                  input.transform_1,
                                  input.transform_2,
                                  input.transform_3);

    float4 pos = mul(float4(input.pos, 1.0f), g_mesh_to_model);
    pos = mul(pos, transform);
    pos = mul(pos, g_proj_view);

    PS_INPUT output;
    output.pos = pos;

    return output;
}

//------------------------------------------------------------------------------
// FRAGMENT SHADER
//------------------------------------------------------------------------------
float4 fragmentShader(PS_INPUT input) : SV_TARGET
{
    return float4(g_radiance, 1.0f);
}
