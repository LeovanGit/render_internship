#include "globals.hlsl"

struct PS_INPUT
{
    float4 posCS : SV_POSITION;
    float3 posWS : POSITION;
    float2 uv : TEXCOORD;    
    float spawn_time : SPAWN_TIME;
    float3 init_velocity : INIT_VELOCITY;
};

struct Particle
{
    float3 position;
    float spawn_time;
    float3 init_velocity;
    float particle_padding_0;
};

// On d3d11 the first valid UAV index is the number of active render targets, so
// the common case of single render target the UAV indexing will start from 1
RWStructuredBuffer<Particle> particles_data : register(u1);

// [0] - begin offset
// [1] - particles count
// [2] - death particles count
RWBuffer<uint> particles_range : register(u2);

Texture2D<float4> g_spark : register(t0);
Texture2D<float> g_depth_buffer : register(t1);

static const float g_SPARK_SIZE = 0.25f;
static const float g_EMISSIVE_POWER = 50.0f;
static const float g_MAX_LIFETIME = 3.0f;
static const float g_APPEARING_TIME = 1.0f; // seconds

//------------------------------------------------------------------------------
// VERTEX SHADER
//------------------------------------------------------------------------------
PS_INPUT vertexShader(uint vertex_index : SV_VERTEXID,
                      uint instance_index : SV_INSTANCEID)
{
    uint spark_index = (particles_range[0] + instance_index) % g_sparks_data_buffer_size;
    float3 posWS = particles_data[spark_index].position;
    float4 posVS = mul(float4(posWS, 1.0f), g_view);
    float half_size = g_SPARK_SIZE / 2.0f;
    
    float2 right = float2(1.0f, 0.0f);
    float2 up = float2(0.0f, 1.0f);

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
    output.spawn_time = particles_data[spark_index].spawn_time;
    output.init_velocity = particles_data[spark_index].init_velocity;
    
    return output;
}

//----------------------------------------------------------------------------
// FRAGMENT SHADER
//------------------------------------------------------------------------------
float4 fragmentShader(PS_INPUT input) : SV_TARGET
{
    float4 color = g_spark.Sample(g_wrap_sampler, input.uv);

    float animation_time = g_time - input.spawn_time;
    float alpha_fading = 1.0f;
    if (animation_time < g_APPEARING_TIME)
    {
        alpha_fading = animation_time / g_APPEARING_TIME;
    }
    else if (animation_time > g_MAX_LIFETIME - g_APPEARING_TIME)
    {
        alpha_fading = (g_MAX_LIFETIME - animation_time) / g_APPEARING_TIME;
    }

    return float4(color.rgb * g_EMISSIVE_POWER, color.a * alpha_fading);
}
