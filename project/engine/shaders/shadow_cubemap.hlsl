cbuffer PerShadowMapMesh : register(b3)
{
    row_major float4x4 g_mesh_to_model;
}

cbuffer PerShadowCamera : register(b4)
{
    row_major float4x4 g_proj_view;
}

struct VS_INPUT
{
    float3 pos : POSITION;

    float4 transform_0 : TRANSFORM0;
    float4 transform_1 : TRANSFORM1;
    float4 transform_2 : TRANSFORM2;
    float4 transform_3 : TRANSFORM3;
};

struct PS_INPUT
{
    float4 pos_CS : SV_POSITION;
};

//------------------------------------------------------------------------------
// VERTEX SHADER
//------------------------------------------------------------------------------
PS_INPUT vertexShader(VS_INPUT input)
{
    float4x4 transform = float4x4(input.transform_0,
                                  input.transform_1,
                                  input.transform_2,
                                  input.transform_3);

    float4 pos_MS = mul(float4(input.pos, 1.0f), g_mesh_to_model);
    float4 pos_WS = mul(pos_MS, transform);
    float4 pos_CS = mul(pos_WS, g_proj_view);

    PS_INPUT output;
    output.pos_CS = pos_CS;

    return output;
}

