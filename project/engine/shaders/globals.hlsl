cbuffer PerFrame : register(b0)
{
    column_major float4x4 g_proj_view;
    float3 g_camera_position;
    float padding_0;
    // 0 - bottom_left, 1 - top_left, 2 - bottom_right
    float4 g_frustum_corners[3];
};

SamplerState g_sampler;