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
    float3 size : SIZE;
};

Texture2D<float4> g_lightmap_RLT : register(t8);
Texture2D<float4> g_lightmap_BotBF : register(t9);
Texture2D<float4> g_motion_vectors : register(t10);
Texture2D<float> g_depth_buffer : register(t11);

static const float g_MV_SCALE = 0.001f;

//------------------------------------------------------------------------------
// VERTEX SHADER
//------------------------------------------------------------------------------
PS_INPUT vertexShader(uint vertex_index: SV_VERTEXID,
                      VS_INPUT input)
{
    float4 posVS = mul(float4(input.posWS, 1.0f), g_view);
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

    float4 posWS_h = mul(float4(vertexVS[vertex_index], posVS.z, 1.0f), g_view_inv);
    
    PS_INPUT output;
    output.posCS = mul(float4(vertexVS[vertex_index], posVS.z, 1.0f), g_proj);
    output.posWS = posWS_h.xyz / posWS_h.w;
    output.uv = vertexUV[vertex_index];
    output.tint = input.tint;
    output.lifetime = input.lifetime;
    output.normal = mul(float4(0.0f, 0.0f, -1.0f, 0.0f), g_view_inv).xyz;
    output.right = mul(float4(right, 0.0, 0.0f), g_view_inv).xyz;
    output.up = mul(float4(up, -1.0f, 0.0f), g_view_inv).xyz;
    output.size = input.size;
    
    return output;
}

//----------------------------------------------------------------------------
// FRAGMENT SHADER
//------------------------------------------------------------------------------
float4 fragmentShader(PS_INPUT input) : SV_TARGET
{
    int sprites_count = g_particles_atlas_size.x * g_particles_atlas_size.y;
    float frame_frac_time = frac(input.lifetime * (sprites_count - 1));
    
    // last particle from atlas lerp with first because of wrap sampler
    int sprite_index = int(input.lifetime * (sprites_count - 1));
    int next_sprite_index = sprite_index + 1;
    
    int2 sprite = int2(sprite_index / g_particles_atlas_size.x,
                       sprite_index % g_particles_atlas_size.y);
    int2 next_sprite = int2(next_sprite_index / g_particles_atlas_size.x,
                            next_sprite_index % g_particles_atlas_size.y);

    float2 uv = (input.uv + float2(sprite.y, sprite.x)) /
                g_particles_atlas_size.x;
    float2 next_uv = (input.uv + float2(next_sprite.y, next_sprite.x)) /
                     g_particles_atlas_size.x;

    float2 mvA = g_motion_vectors.Sample(g_wrap_sampler, uv).gb;
    float2 mvB = g_motion_vectors.Sample(g_wrap_sampler, next_uv).gb;
    mvA *= 2.0f - 1.0f;
    mvB *= 2.0f - 1.0f;
    
    float2 uvA = uv;
    uvA -= mvA * g_MV_SCALE * frame_frac_time;

    float2 uvB = next_uv;
    uvB -= mvB * g_MV_SCALE * (frame_frac_time - 1.0f);
        
    float3 valueA = g_lightmap_RLT.Sample(g_wrap_sampler, uvA).rgb;
    float3 valueB = g_lightmap_RLT.Sample(g_wrap_sampler, uvB).rgb;
    float3 lightmap_RLT = lerp(valueA, valueB, frame_frac_time);

    valueA = g_lightmap_BotBF.Sample(g_wrap_sampler, uvA).rgb;
    valueB = g_lightmap_BotBF.Sample(g_wrap_sampler, uvB).rgb;
    float3 lightmap_BotBF = lerp(valueA, valueB, frame_frac_time);
    
    float3 color = calculateLighting(input.posWS,
                                     normalize(input.right),
                                     normalize(input.up),
                                     normalize(input.normal),
                                     lightmap_RLT,
                                     lightmap_BotBF);

    float alpha = lerp(g_motion_vectors.Sample(g_wrap_sampler, uvA).a,
                       g_motion_vectors.Sample(g_wrap_sampler, uvB).a,
                       frame_frac_time);
    
    float scene_depth = g_depth_buffer.Load(int3(input.posCS.xy, 0)).r;

    float2 scene_posCS = float2((input.posCS.x / g_screen_size.x) * 2.0f - 1.0f,
                                1.0f - 2.0f * (input.posCS.y / g_screen_size.y));
    
    float4 scene_posWS_h = mul(float4(scene_posCS, scene_depth, 1.0f), g_proj_view_inv);
    float3 scene_posWS = scene_posWS_h.xyz / scene_posWS_h.w;
    
    float particle_fading = saturate(length(scene_posWS - input.posWS) / input.size.z);

    return float4(color, alpha * particle_fading) * input.tint;
}
