#include "globals.hlsl"

struct PS_INPUT
{
    float4 pos : SV_POSITION;  
};

Texture2D<float4> g_ldr_scene : register(t0);

static const row_major float3x3 weights = float3x3(0.0453542f, 0.0566406f, 0.0453542f,
                                                   0.0566406f, 0.0707355f, 0.0566406f,
                                                   0.0453542f, 0.0566406f, 0.0453542f);

//------------------------------------------------------------------------------
// VERTEX SHADER
//------------------------------------------------------------------------------
PS_INPUT vertexShader(uint vertex_index : SV_VERTEXID)
{
    // generate fullscreen triangle in clip space
    float2 vertexCS[3] = {{-1.0f, -1.0f},
                          {-1.0f,  3.0f},
                          { 3.0f, -1.0f}};

    PS_INPUT output;
    output.pos = float4(vertexCS[vertex_index], 1.0f, 1.0f);

    return output;
}

//------------------------------------------------------------------------------
// FRAGMENT SHADER
//------------------------------------------------------------------------------
float4 fragmentShader(PS_INPUT input) : SV_TARGET
{
    float3 color = 0.0f;
    float weights_sum = 0.0f;
    
    for (uint row = 0; row != 3; ++row)
    {
        for (uint col = 0; col != 3; ++col)
        {
            float2 pos = input.pos.xy + float2(row, col);

            // edges
            if (pos.x < 0) pos.x = -pos.x;
            if (pos.y < 0) pos.y = -pos.y;
            if (pos.x >= g_screen_size.x) pos.x -= 2;
            if (pos.y >= g_screen_size.y) pos.y -= 2;
            
            color += g_ldr_scene.Load(int3(pos, 0)) * weights[row][col];
            weights_sum += weights[row][col];
        }
    }
    color /= weights_sum;
    
    return float4(color, 1.0f);
}
