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
// Texture2D<float> g_depth_buffer : register(t1); bind depth with RT

static const float g_SPARK_SIZE = 1.0f;
static const uint g_SPARKS_DATA_BUFFER_SIZE = 150000;

//------------------------------------------------------------------------------
// VERTEX SHADER
//------------------------------------------------------------------------------
PS_INPUT vertexShader(uint vertex_index : SV_VERTEXID,
                      uint instance_index : SV_INSTANCEID)
{
    uint spark_index = (particles_range[0] + instance_index) % g_SPARKS_DATA_BUFFER_SIZE;
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
    //float scene_depth = g_depth_buffer.Load(int3(input.posCS.xy, 0)).r;
    
    return color;
    
    /* float2 scene_posCS = float2((input.posCS.x / g_screen_size.x) * 2.0f - 1.0f, */
    /*                             1.0f - 2.0f * (input.posCS.y / g_screen_size.y)); */
    
    /* float4 scene_posWS_h = mul(float4(scene_posCS, scene_depth, 1.0f), g_proj_view_inv); */
    /* float3 scene_posWS = scene_posWS_h.xyz / scene_posWS_h.w; */
    
    /* float particle_fading = saturate(length(scene_posWS - input.posWS) / input.size.z); */

    /* return float4(color, alpha * particle_fading) * input.tint; */
}
