#include "globals.hlsl"

cbuffer PerMesh : register(b1)
{
    row_major float4x4 g_mesh_to_model;

    bool g_has_albedo_texture;
    bool g_has_roughness_texture;
    bool g_has_metalness_texture;
    bool g_has_normal_map;
    bool g_is_directx_style_normal_map;
    float3 padding_0;
    
    float3 g_albedo_default;
    float g_roughness_default;
    float g_metalness_default;
    float3 padding_1;
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

    uint model_id : MODEL_ID;
    float model_box_diameter : BOX_DIAMETER;
    float spawn_time : SPAWN_TIME;
    float animation_time : ANIMATION_TIME;
    float3 sphere_origin : SPHERE_ORIGIN;
};

struct Particle
{
    float3 position;
    float spawn_time;
    float3 velocity;
    float particle_padding_0;
};

// On d3d11 the first valid UAV index is the number of active render targets, so
// the common case of single render target the UAV indexing will start from 1
RWStructuredBuffer<Particle> particles_data : register(u1);

// [0] - begin offset
// [1] - particles count
// [2] - death particles count
RWBuffer<uint> particles_range : register(u2);

static const float g_INIT_SPARK_VELOCITY = 15.0f;
static const uint g_SPARKS_DATA_BUFFER_SIZE = 150000;

//------------------------------------------------------------------------------
// VERTEX SHADER
//------------------------------------------------------------------------------
float4 vertexShader(VS_INPUT input) : SV_POSITION
{    
    float4x4 transform = float4x4(input.transform_0,
                                  input.transform_1,
                                  input.transform_2,
                                  input.transform_3);

    float4 pos_MS = mul(float4(input.pos, 1.0f), g_mesh_to_model);
    float4 pos_WS = mul(pos_MS, transform);
    float4 pos_CS = mul(pos_WS, g_proj_view);

    float dist = length(pos_WS.xyz - input.sphere_origin);
    // time after which sphere radius will be equal dist
    float spawn_time = input.spawn_time +
                       dist * input.animation_time / input.model_box_diameter;
    // g_time of the previous frame
    float prev_time = g_time - g_delta_time;
    
    if (prev_time < spawn_time && spawn_time < g_time)
    {
        uint prev_count;
        InterlockedAdd(particles_range[1], 1, prev_count);
        
        Particle particle;
        particle.position = pos_WS.xyz;
        particle.spawn_time = g_time;
        particle.velocity = input.normal * g_INIT_SPARK_VELOCITY;
        particle.particle_padding_0 = 0;

        particles_data[(particles_range[0] + prev_count) % g_SPARKS_DATA_BUFFER_SIZE] = particle;
    }

    return pos_CS;
}

