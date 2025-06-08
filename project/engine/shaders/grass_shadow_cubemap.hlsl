#include "globals.hlsl"

cbuffer PerShadowCubemap : register(b5)
{
    int g_cubemap_index;
    float3 padding_4;
}

struct VS_INPUT
{
    float3 posWS : POSITION;
    float2 size : SIZE;
};

struct GS_INPUT
{
    float4 posWS : SV_POSITION;
    float2 uv : UV;
};

struct PS_INPUT
{
    float4 posCS : SV_POSITION;
    nointerpolation uint slice : SV_RenderTargetArrayIndex;
    
    float2 uv : UV;
};

Texture2D<float> g_grass_opacity : register(t0);

//------------------------------------------------------------------------------
// VERTEX SHADER
//------------------------------------------------------------------------------
GS_INPUT vertexShader(uint vertex_index: SV_VERTEXID,
                      VS_INPUT input)
{
    float2 half_size = input.size.xy / 2.0f;

    int plane_index = int(vertex_index / 6.0f);

    float angle = (1.0f / g_PLANES_PER_GRASS) * plane_index * g_PI;
    float sina, cosa;
    sincos(angle, sina, cosa);
    
    float4x4 rotate = float4x4(cosa, 0.0f, sina, 0.0f,
                               0.0f, 1.0f, 0.0f, 0.0f,
                               -sina, 0.0f, cosa, 0.0f,
                               0.0f, 0.0f, 0.0f, 1.0f);

    float4x4 translate = float4x4(1.0f, 0.0f, 0.0f, 0.0f,
                                  0.0f, 1.0f, 0.0f, 0.0f,
                                  0.0f, 0.0f, 1.0f, 0.0f,
                                  input.posWS, 1.0f);   
    
    float3 planeMS[6] = {float3(-half_size.x, -half_size.y, 0.0f),
                         float3(-half_size.x, +half_size.y, 0.0f),
                         float3(+half_size.x, +half_size.y, 0.0f),

                         float3(+half_size.x, +half_size.y, 0.0f),
                         float3(+half_size.x, -half_size.y, 0.0f),
                         float3(-half_size.x, -half_size.y, 0.0f)};

    float2 uv[6] = {{0.0f, 1.0f},
                    {0.0f, 0.0f},
                    {1.0f, 0.0f},

                    {1.0f, 0.0f},
                    {1.0f, 1.0f},
                    {0.0f, 1.0f}};

    float4 posWS = mul(float4(planeMS[vertex_index % 6.0f], 1.0f), rotate);
    posWS = mul(posWS, translate);

    GS_INPUT output;
    output.posWS = posWS;
    output.uv = uv[vertex_index % 6.0f];

    return output;
}

//------------------------------------------------------------------------------
// GEOMETRY SHADER
//------------------------------------------------------------------------------
[maxvertexcount(18)]
void geometryShader(triangle GS_INPUT input[3],
                    inout TriangleStream<PS_INPUT> output_stream)
{
    // generate 6 triangles for each camera from 1 main triangle
    for (uint face = 0; face != 6; ++face)
    {
        for (uint i = 0; i != 3; ++i)
        {
            PS_INPUT output;
            output.slice = g_cubemap_index * 6 + face;
            output.posCS = mul(input[i].posWS,
                               g_light_proj_view[g_cubemap_index * 6 + face]);

            output.uv = input[i].uv;

            output_stream.Append(output);
        }
        output_stream.RestartStrip();
    }
}

//------------------------------------------------------------------------------
// FRAGMENT SHADER
//------------------------------------------------------------------------------
float fragmentShader(PS_INPUT input) : SV_DEPTH
{
    float opacity = g_grass_opacity.Sample(g_wrap_sampler, input.uv);

    if (abs(opacity) < g_EPSILON)
    {
        discard;
        return 0.0f;
    }
    else return input.posCS.z;
}
