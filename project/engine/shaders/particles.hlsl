#include "globals.hlsl"
#include "lighting.hlsl"

struct VS_INPUT
{
    float3 posWS : POSITION;
    float3 size : SIZE;
    float angle : ANGLE;
    float4 tint : TINT;
    float lifetime : LIFETIME;
};

struct PS_INPUT
{
    float4 posCS : SV_POSITION;
    float3 posWS : POSITION;
    float2 uv : TEXCOORD;
    float4 tint : TINT;
    float lifetime : LIFETIME;
    float3 normal : NORMAL;
    float3 right : RIGHT;
    float3 up : UP;
};

Texture2D<float4> g_lightmap_RLT : register(t8);
Texture2D<float4> g_lightmap_BotBF : register(t9);
Texture2D<float4> g_motion_vectors : register(t10);

//------------------------------------------------------------------------------
// VERTEX SHADER
//------------------------------------------------------------------------------
PS_INPUT vertexShader(uint vertex_index: SV_VERTEXID,
                      VS_INPUT input)
{
    float3 posVS = mul(float4(input.posWS, 1.0f), g_view).xyz;
    float half_size = input.size.x / 2.0f;
    
    float sina;
    float cosa;
    sincos(input.angle, sina, cosa);
    
    row_major float2x2 rotate_matrix = float2x2(cosa, -sina,
                                                sina,  cosa);
    
    float2 right = mul(float2(1.0f, 0.0f), rotate_matrix);
    float2 up = mul(float2(0.0f, 1.0f), rotate_matrix);

    // generate WS particle quad oriented on camera
    float2 vertexVS[6] = {{posVS.xy - (right + up) * half_size},
                          {posVS.xy - (right - up) * half_size},
                          {posVS.xy + (right + up) * half_size},

                          {posVS.xy + (right + up) * half_size},
                          {posVS.xy + (right - up) * half_size},
                          {posVS.xy - (right + up) * half_size}};

    float2 vertexUV[6] = {{0.0f, 1.0f},
                          {0.0f, 0.0f},
                          {1.0f, 0.0f},

                          {1.0f, 0.0f},
                          {1.0f, 1.0f},
                          {0.0f, 1.0f}};
    
    PS_INPUT output;
    output.posCS = mul(float4(vertexVS[vertex_index], posVS.z, 1.0f), g_proj);
    output.posWS = mul(float4(vertexVS[vertex_index], posVS.z, 1.0f), g_view_inv).xyz;
    output.uv = vertexUV[vertex_index];
    output.tint = input.tint;
    output.lifetime = input.lifetime;
    output.normal = mul(float4(0.0f, 0.0f, -1.0f, 0.0f), g_view_inv).xyz;
    output.right = mul(float4(right, 0.0, 0.0f), g_view_inv).xyz;
    output.up = mul(float4(up, -1.0f, 0.0f), g_view_inv).xyz;

    return output;
}

//------------------------------------------------------------------------------
// FRAGMENT SHADER
//------------------------------------------------------------------------------
float4 fragmentShader(PS_INPUT input) : SV_TARGET
{    
    int sprites_count = g_particles_atlas_size.x * g_particles_atlas_size.y;
    int sprite_index = int(input.lifetime * (sprites_count - 1));
    int2 sprite = int2(sprite_index / g_particles_atlas_size.x,
                       sprite_index % g_particles_atlas_size.y);
    
    float2 uv = (input.uv + float2(sprite.y, sprite.x)) / 8.0f;

    float3 lightmap_RLT = g_lightmap_RLT.Sample(g_wrap_sampler, uv).rgb;
    float3 lightmap_BotBF = g_lightmap_BotBF.Sample(g_wrap_sampler, uv).rgb;
    float4 motion_vectors = g_motion_vectors.Sample(g_wrap_sampler, uv);

    float3 color = calculateLighting(input.posWS,
                                     input.right,
                                     input.up,
                                     input.normal,
                                     lightmap_RLT,
                                     lightmap_BotBF);

    return float4(color, motion_vectors.a) * input.tint;
}
