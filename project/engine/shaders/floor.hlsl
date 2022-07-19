#include "globals.hlsl"

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

Texture2D g_texture;

//------------------------------------------------------------------------------
// VERTEX SHADER
//------------------------------------------------------------------------------
PS_INPUT vertexShader(uint vertex_index: SV_VERTEXID)
{    
    float floor_size = 200.0f;
    float floor_y = -10.0f;
    float3 vertexWS[6] = {{-floor_size, floor_y, -floor_size},
                          { floor_size, floor_y,  floor_size},
                          { floor_size, floor_y, -floor_size},
                          
                          { floor_size, floor_y,  floor_size},
                          {-floor_size, floor_y, -floor_size},
                          {-floor_size, floor_y,  floor_size}};

    float tex_size = floor_size / 20.0f;
    float2 uv[6] = {{0.0f, tex_size},
                    {tex_size, 0.0f},
                    {tex_size, tex_size},

                    {tex_size, 0.0f},
                    {0.0f, tex_size},
                    {0.0f, 0.0f}};

    PS_INPUT output;
    output.pos = mul(g_proj_view, float4(vertexWS[vertex_index], 1.0f));
    output.uv = uv[vertex_index];
    
    return output;
}

//------------------------------------------------------------------------------
// FRAGMENT SHADER
//------------------------------------------------------------------------------
float4 fragmentShader(PS_INPUT input) : SV_Target
{
    float3 color = g_texture.Sample(g_sampler, input.uv);
    return float4(color, 1.0f);
}
