#ifndef GLOBALS_HLSL
#define GLOBALS_HLSL

SamplerState g_wrap_sampler : register(s0);
SamplerState g_clamp_sampler : register(s1);
SamplerComparisonState g_comparison_sampler : register(s2);

static const float g_GAMMA = 2.2f;
static const float g_PI = 3.14159265f;
static const float g_F0_DIELECTRIC = 0.04f;

static const uint g_POINT_LIGHTS_COUNT = 4;
static const uint g_DIR_LIGHTS_COUNT = 1;

cbuffer PerFrame : register(b0)
{
    struct
    {
        float3 position;
        float padding_0;
        float3 radiance;
        float radius;
    } g_point_lights[g_POINT_LIGHTS_COUNT];

    struct
    {
        float3 direction;
        float padding_0;
        float3 radiance;
        float solid_angle;
    } g_dir_lights[g_DIR_LIGHTS_COUNT];

    int g_reflection_mips_count;
    int g_shadow_map_size;
    int2 g_particles_atlas_size;

    int g_samples_count;

    int2 g_screen_size;
    int padding_per_frame_0;
    
    row_major float4x4 g_light_proj_view[24]; // 4 cubemaps
};

cbuffer PerView : register(b4)
{
    row_major float4x4 g_proj_view;
    row_major float4x4 g_proj_view_inv;
    row_major float4x4 g_view;
    row_major float4x4 g_view_inv;
    row_major float4x4 g_proj;
    row_major float4x4 g_proj_inv;
    float3 g_camera_position;
    float g_EV_100;
    // 0 - bottom_left, 1 - top_left, 2 - bottom_right
    float4 g_frustum_corners[3];
};

#endif
