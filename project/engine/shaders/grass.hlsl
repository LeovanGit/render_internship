#include "globals.hlsl"
#include "lighting.hlsl"

struct VS_INPUT
{
    float3 posWS : POSITION;
    float2 size : SIZE;
};

struct PS_INPUT
{
    float4 posCS : SV_POSITION;
    float2 uv : UV;
};

Texture2D<float3> g_albedo : register(t0);
Texture2D<float> g_opacity : register(t1);

static const float g_PLANES_PER_GRASS = 3;

//------------------------------------------------------------------------------
// VERTEX SHADER
//------------------------------------------------------------------------------
PS_INPUT vertexShader(uint vertex_index: SV_VERTEXID,
                      VS_INPUT input)
{
    float2 half_size = input.size.xy / 2.0f;

    int plane_index = int(vertex_index / 6.0f);

    float angle = (1.0f / g_PLANES_PER_GRASS) * plane_index * g_PI;
    float sina, cosa;
    sincos(angle, sina, cosa);
    
    float4x4 rotate = float4x4(cosa, 0.0f, sina, 0.0f,
                               0.0f, 1.0f, 0.0f, 0.0f,
                               -sina, 0.0f, cosa, 0.0f,
                               0.0f, 0.0f, 0.0f, 1.0f);

    float4x4 translate = float4x4(1.0f, 0.0f, 0.0f, 0.0f,
                                  0.0f, 1.0f, 0.0f, 0.0f,
                                  0.0f, 0.0f, 1.0f, 0.0f,
                                  input.posWS, 1.0f);   
    
    float3 planeMS[6] = {float3(-half_size.x, -half_size.y, 0.0f),
                         float3(-half_size.x, +half_size.y, 0.0f),
                         float3(+half_size.x, +half_size.y, 0.0f),

                         float3(+half_size.x, +half_size.y, 0.0f),
                         float3(+half_size.x, -half_size.y, 0.0f),
                         float3(-half_size.x, -half_size.y, 0.0f)};

    float2 uv[6] = {{0.0f, 1.0f},
                    {0.0f, 0.0f},
                    {1.0f, 0.0f},

                    {1.0f, 0.0f},
                    {1.0f, 1.0f},
                    {0.0f, 1.0f}};

    float4 posWS = mul(float4(planeMS[vertex_index % 6.0f], 1.0f), rotate);
    posWS = mul(posWS, translate);

    PS_INPUT output;
    output.posCS = mul(posWS, g_proj_view);
    output.uv = uv[vertex_index % 6.0f];

    return output;
}

//------------------------------------------------------------------------------
// FRAGMENT SHADER
//------------------------------------------------------------------------------
float4 fragmentShader(PS_INPUT input) : SV_TARGET
{
    float3 albedo = g_albedo.Sample(g_wrap_sampler, input.uv);
    float opacity = g_opacity.Sample(g_wrap_sampler, input.uv);
    
    return float4(albedo, opacity);
}
