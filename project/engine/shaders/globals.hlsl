#ifndef GLOBALS_HLSL
#define GLOBALS_HLSL

SamplerState g_sampler;

static const float g_gamma = 2.2f;
static const float g_PI = 3.14159265f;
static const float g_F0_dielectric = 0.04f;

static const uint g_point_lights_count = 4;
static const uint g_dir_lights_count = 1;

cbuffer PerFrame : register(b0)
{
    row_major float4x4 g_proj_view;
    float3 g_camera_position;
    float g_EV_100;
    // 0 - bottom_left, 1 - top_left, 2 - bottom_right
    float4 g_frustum_corners[3];

    struct
    {
        float3 position;
        float padding_0;
        float3 radiance;
        float radius;
    } g_point_lights[g_point_lights_count];

    struct
    {
        float3 direction;
        float padding_0;
        float3 radiance;
        float solid_angle;
    } g_dir_lights[g_dir_lights_count];
};

#endif
