#include "globals.hlsl"

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
    float2 uv : TEXCOORD;
    float4 tint : TINT;
    float lifetime : LIFETIME;
};

Texture2D<float4> g_particle : register(t8);

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
    output.uv = vertexUV[vertex_index];
    output.tint = input.tint;
    output.lifetime = input.lifetime;

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
    
    return input.tint * g_particle.Sample(g_wrap_sampler, uv);
}
