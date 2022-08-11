#include "globals.hlsl"

cbuffer PerMesh : register(b1)
{
    row_major float4x4 g_mesh_to_model;
    
    bool g_has_roughness_texture;
    bool g_has_metalness_texture;
    bool g_has_normal_map;

    float g_roughness_default;
    float g_metalness_default;
    float3 g_normal_default;
}

struct VS_INPUT
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    
    float4 transform_0 : TRANSFORM0;
    float4 transform_1 : TRANSFORM1;
    float4 transform_2 : TRANSFORM2;
    float4 transform_3 : TRANSFORM3;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

Texture2D g_albedo : register(t0);
Texture2D g_roughness : register(t1);
Texture2D g_metalness : register(t2);
Texture2D g_normal : register(t3);

// tmp point light values
float3 light_pos = float3(0.0f, 0.0f, 0.0f);
float3 light_radiance = float3(3000.0f, 3000.0f, 3000.0f);
float radius = 50.0f;

//------------------------------------------------------------------------------
// VERTEX SHADER
//------------------------------------------------------------------------------
PS_INPUT vertexShader(VS_INPUT input)
{
    float4x4 transform = float4x4(input.transform_0,
                                  input.transform_1,
                                  input.transform_2,
                                  input.transform_3);
    
    PS_INPUT output;
    output.uv = input.uv;

    float4 pos = mul(float4(input.pos, 1.0f), g_mesh_to_model);
    pos = mul(pos, transform);
    pos = mul(pos, g_proj_view);
    output.pos = pos;

    return output;
}

//------------------------------------------------------------------------------
// FRAGMENT SHADER
//------------------------------------------------------------------------------
// G
float ggxSmith(float roughness_sqr,
               float NL,
               float NV);

// D
float ggxTrowbridgeReitz(float roughness_sqr,
                         float NH);

// F
float3 ggxSchlick(float cosTheta,
                  float3 F0);

float4 fragmentShader(PS_INPUT input) : SV_TARGET
{
    float3 color = g_albedo.Sample(g_sampler, input.uv);
    
    return float4(color, 1.0f);
}

//------------------------------------------------------------------------------

float ggxSmith(float roughness_sqr,
               float NL,
               float NV)
{
    float NL_sqr = NL * NL;
    float NV_sqr = NV * NV;

    // fast Smith from Filament
    return 2.0f / (sqrt(1 + roughness_sqr * (1 - NV_sqr) / NV_sqr) +
                   sqrt(1 + roughness_sqr * (1 - NL_sqr) / NL_sqr));

}

float ggxTrowbridgeReitz(float roughness_sqr,
                         float NH)
{
    float NH_sqr = NH * NH;
    float denom = NH_sqr * (roughness_sqr - 1.0f) + 1.0f;
    float D = roughness_sqr / (g_PI  * denom * denom);

    return D;
}

float3 ggxSchlick(float cosTheta,
                  float3 F0)
{
    return F0 + (1.0f - F0) * pow(1.0f - cosTheta, 5.0f);
}
