//------------------------------------------------------------------------------
cbuffer PerFrame : register(b0)
{
    row_major float4x4 g_proj_view : packoffset(c0);
};

struct VS_INPUT
{
    float3 pos : POSITION;
    float4 color : COLOR;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};

//------------------------------------------------------------------------------
// VERTEX SHADER
//------------------------------------------------------------------------------
PS_INPUT vertexShader(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT)0;

    output.color = input.color;
    output.pos = mul(float4(input.pos, 1.0f), g_proj_view);

    return output;
}

//------------------------------------------------------------------------------
// FRAGMENT SHADER
//------------------------------------------------------------------------------
float4 fragmentShader(PS_INPUT input) : SV_Target
{
    return input.color;
}
