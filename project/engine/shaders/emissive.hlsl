#include "globals.hlsl"
#include "math.hlsl"

cbuffer PerEmissiveMesh : register(b2)
{
    row_major float4x4 g_mesh_to_model;
}

struct VS_INPUT
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    
    float4 transform_0 : TRANSFORM0;
    float4 transform_1 : TRANSFORM1;
    float4 transform_2 : TRANSFORM2;
    float4 transform_3 : TRANSFORM3;

    float4 radiance : RADIANCE;
};

struct PS_INPUT
{
    float4 pos_CS : SV_POSITION;
    float3 pos_WS : POSITION;
    float3 normal : NORMAL;
    float3 radiance : RADIANCE;
    float4x4 transform : TRANSFORM;
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

    float4 pos_MS = mul(float4(input.pos, 1.0f), g_mesh_to_model);
    float4 pos_WS = mul(pos_MS, transform);
    float4 pos_CS = mul(pos_WS, g_proj_view);

    PS_INPUT output;
    output.pos_CS = pos_CS;
    output.pos_WS = pos_WS.xyz;
    output.normal = input.normal;
    output.radiance = input.radiance.xyz;
    output.transform = transform;

    return output;
}

//------------------------------------------------------------------------------
// FRAGMENT SHADER
//------------------------------------------------------------------------------
float4 fragmentShader(PS_INPUT input) : SV_TARGET
{
    float3 N = normalize(input.normal);
    N = normalize(mul(float4(N, 0.0f), g_mesh_to_model).xyz);
    N = normalize(mul(float4(N, 0.0f), input.transform).xyz);
    
    float3 V = normalize(g_camera_position - input.pos_WS);
    float NV = max(dot(N, V), 0.001f);

    float3 radiance_norm = input.radiance / max3(input.radiance);
    
    float3 color = lerp(radiance_norm, input.radiance, pow(NV, 6));    
    
    return float4(color, 1.0f);
}

