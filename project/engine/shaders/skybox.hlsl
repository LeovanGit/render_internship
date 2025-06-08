#include "globals.hlsl"

struct PS_INPUT
{
    float4 posCS : SV_POSITION;
    float3 dirWS : DIRECTION_WS;
};

TextureCube g_skybox;

//------------------------------------------------------------------------------
// VERTEX SHADER
//------------------------------------------------------------------------------
PS_INPUT vertexShader(uint vertex_index: SV_VERTEXID)
{
    // generate fullscreen triangle in clip space
    float2 vertexCS[3] = {{-1.0f, -1.0f},
                          {-1.0f,  3.0f},
                          { 3.0f, -1.0f}};

    PS_INPUT output;
    // 0.0f -> draw skybox on far frustum plane
    output.posCS = float4(vertexCS[vertex_index], 0.0f, 1.0f);
   
    float3 dx = g_frustum_corners[2] - g_frustum_corners[0];
    float3 dy = g_frustum_corners[1] - g_frustum_corners[0];

    float2 uv = (vertexCS[vertex_index] + float2(1.0f, 1.0f)) / 2.0f;

    output.dirWS = g_frustum_corners[0].xyz +
                   uv.x * dx +
                   uv.y * dy -
                   g_camera_position;

    return output;
}

//------------------------------------------------------------------------------
// FRAGMENT SHADER
//------------------------------------------------------------------------------
float4 fragmentShader(PS_INPUT input) : SV_TARGET
{
    float3 color = g_skybox.Sample(g_wrap_sampler, input.dirWS);
    return float4(color, 1.0f);
}
