#include "globals.hlsl"
#include "math.hlsl"

struct Particle
{
    float3 position;
    float spawn_time;
    float3 velocity;
    float particle_padding_0;
};

RWStructuredBuffer<Particle> particles_data : register(u1);

// [0] - begin offset
// [1] - particles count
// [2] - death particles count
RWBuffer<uint> particles_range : register(u2);

Texture2D<float> g_depth_buffer : register(t0);
Texture2D<float4> g_normals : register(t1);

static const float g_MAX_LIFETIME = 3.0f;
static const float g_GRAVITATION = 20.0f;
static const float g_THRESHOLD = 0.5f;
static const float g_REDUCE_ENERGY = 0.5f;

[numthreads(64, 1, 1)]
void computeShader(uint3 thread_id : SV_DispatchThreadID)
{
    if (thread_id.x >= particles_range[1]) return;

    uint spark_index = (particles_range[0] + thread_id.x) % g_sparks_data_buffer_size;

    float animation_time = g_time - particles_data[spark_index].spawn_time;
    if (animation_time > g_MAX_LIFETIME)
    {
        InterlockedAdd(particles_range[2], 1);
        return;
    }
    
    particles_data[spark_index].position += particles_data[spark_index].velocity * g_delta_time;
    particles_data[spark_index].velocity.y -= g_GRAVITATION * g_delta_time;

    float3 posWS = particles_data[spark_index].position;
    float4 posCS_h = mul(float4(posWS, 1.0f), g_proj_view);
    float3 posCS = posCS_h.xyz / posCS_h.w;

    float2 pos_viewport = float2(((posCS.x + 1.0f) / 2.0f) * g_screen_size.x,
                                 ((1.0f - posCS.y) / 2.0f) * g_screen_size.y);
        
    float scene_depth = g_depth_buffer.Load(int3(pos_viewport, 0)).r;
    float4 scene_posWS_h = mul(float4(posCS.xy, scene_depth, 1.0f), g_proj_view_inv);
    float3 scene_posWS = scene_posWS_h.xyz / scene_posWS_h.w;

    float dist = length(posWS - scene_posWS);
    if (dist < g_THRESHOLD)
    {
        float3 scene_normal = unpackOctahedron(g_normals.Load(int3(pos_viewport, 0)).ba);

        // because particle can fall through (or stuck in) geometry
        particles_data[spark_index].position += scene_normal * 1.1f * dist;
        
        particles_data[spark_index].velocity =
            reflect(particles_data[spark_index].velocity, scene_normal);

        particles_data[spark_index].velocity *= g_REDUCE_ENERGY;       
    }
}
