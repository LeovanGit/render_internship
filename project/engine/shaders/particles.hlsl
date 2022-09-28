#include "globals.hlsl"

struct VS_INPUT
{
    float3 posWS : POSITION;
    float3 size : SIZE;
    float angle : ANGLE;
    float4 tint : TINT;
};

struct PS_INPUT
{
    float4 posCS : SV_POSITION;
    float2 uv : TEXCOORD;
    float4 tint : TINT;
};

Texture2D<float4> g_particle : register(t8);

//------------------------------------------------------------------------------
// VERTEX SHADER
//------------------------------------------------------------------------------
PS_INPUT vertexShader(uint vertex_index: SV_VERTEXID,
                      VS_INPUT input)
{    
    float3 camera_right = normalize((g_frustum_corners[2] - g_frustum_corners[0]).xyz);
    float3 camera_up = normalize((g_frustum_corners[1] - g_frustum_corners[0]).xyz);

    float half_size = input.size.x / 2.0f;

    // generate WS particle quad oriented on camera
    float3 vertexWS[6] = {{input.posWS - (camera_right + camera_up) * half_size},
                          {input.posWS - (camera_right - camera_up) * half_size},
                          {input.posWS + (camera_right + camera_up) * half_size},

                          {input.posWS + (camera_right + camera_up) * half_size},
                          {input.posWS + (camera_right - camera_up) * half_size},
                          {input.posWS - (camera_right + camera_up) * half_size}};

    float2 vertexUV[6] = {{0.0f, 1.0f},
                          {0.0f, 0.0f},
                          {1.0f, 0.0f},

                          {1.0f, 0.0f},
                          {1.0f, 1.0f},
                          {0.0f, 1.0f}};

    float sina;
    float cosa;
    sincos(input.angle, sina, cosa);

    /* row_major float2x2 rotate_matrix = float2x2(cosa, -sina, */
    /*                                             sina,  cosa); */

    float4 posCS = mul(float4(vertexWS[vertex_index], 1.0f), g_proj_view);
    //posCS.xy = mul(posCS.xy, rotate_matrix);
    
    PS_INPUT output;
    output.posCS = posCS;
    output.uv = vertexUV[vertex_index];
    output.tint = input.tint;

    return output;
}

//------------------------------------------------------------------------------
// FRAGMENT SHADER
//------------------------------------------------------------------------------
float4 fragmentShader(PS_INPUT input) : SV_TARGET
{
    //return input.tint * g_particle.Sample(g_wrap_sampler, input.uv);
    return input.tint;
}
