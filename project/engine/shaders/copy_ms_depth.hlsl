#include "globals.hlsl"

struct PS_INPUT
{
    float4 pos : SV_POSITION;
};

Texture2DMS<float> g_scene_depth : register(t12);

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
    float depth = 1.0f;
    for (uint i = 0; i != g_samples_count; ++i)
    {
        float sample = g_scene_depth.Load(int3(input.pos.xy, 0), i);

        if (sample < depth) depth = sample;
    }
    
    return float4(depth, depth, depth, 1.0f);
}
