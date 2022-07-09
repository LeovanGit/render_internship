//------------------------------------------------------------------------------
cbuffer PerFrame : register(b0)
{
    row_major float4x4 g_proj_view : packoffset(c0);
};

struct VS_INPUT
{
    float3 pos : POSITION;
    float2 uv : UV;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float2 uv : UV;
};

Texture2D g_texture : TEXTURE : register(t0);
SamplerState g_sampler_state : SAMPLER : register(s0);

//------------------------------------------------------------------------------
// VERTEX SHADER
//------------------------------------------------------------------------------
PS_INPUT vertexShader(VS_INPUT input)
{
    PS_INPUT output;
    output.uv = input.uv;
    output.pos = mul(float4(input.pos, 1.0f), g_proj_view);

    return output;
}

//------------------------------------------------------------------------------
// FRAGMENT SHADER
//------------------------------------------------------------------------------
float4 fragmentShader(PS_INPUT input) : SV_Target
{
    float3 color = g_texture.Sample(g_sampler_state, input.uv);

    return float4(color, 1.0f);
}
