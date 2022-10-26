#include "globals.hlsl"
#include "math.hlsl"

struct VS_INPUT
{
    float3 posWS : POSITION;
    float2 size : SIZE;
    float3 albedo : ALBEDO;
};

struct PS_INPUT
{
    float4 posCS : SV_POSITION;
    float3 albedo : ALBEDO;
};

struct PS_OUTPUT
{
    float4 normals : SV_TARGET0;
    float3 albedo : SV_TARGET1;
    float2 roughness_metalness : SV_TARGET2;
    float4 emissive_ao : SV_TARGET3;
};

Texture2D<float3> g_decal_normals : register(t0);

//------------------------------------------------------------------------------
// VERTEX SHADER
//------------------------------------------------------------------------------
PS_INPUT vertexShader(uint vertex_index: SV_VERTEXID,
                      VS_INPUT input)
{
    float2 half_size = input.size.xy / 2.0f;

    //vertexMS[6] = {}
    float3 vertexMS[6] = {float3(-half_size.x, -half_size.y, 0.0f),
                          float3(-half_size.x, +half_size.y, 0.0f),
                          float3(+half_size.x, +half_size.y, 0.0f),

                          float3(+half_size.x, +half_size.y, 0.0f),
                          float3(+half_size.x, -half_size.y, 0.0f),
                          float3(-half_size.x, -half_size.y, 0.0f)};

    float4x4 trans = translate(input.posWS);

    float4 posWS = mul(float4(vertexMS[vertex_index], 1.0f), trans);
    
    PS_INPUT output;

    output.posCS = mul(posWS, g_proj_view);
    output.albedo = input.albedo;

    return output;
}

//------------------------------------------------------------------------------
// FRAGMENT SHADER
//------------------------------------------------------------------------------
PS_OUTPUT fragmentShader(PS_INPUT input)
{
    PS_OUTPUT output;

    output.albedo = float4(1.0f, 0.0f, 0.0f, 1.0f);

    return output;
}
