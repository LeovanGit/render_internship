cbuffer PerShadowMapMesh : register(b3)
{
    row_major float4x4 g_mesh_to_model;
}

cbuffer PerShadowCamera : register(b4)
{
    row_major float4x4 g_proj_view[6];
}

struct VS_INPUT
{
    float3 pos : POSITION;

    float4 transform_0 : TRANSFORM0;
    float4 transform_1 : TRANSFORM1;
    float4 transform_2 : TRANSFORM2;
    float4 transform_3 : TRANSFORM3;
};

struct GS_INPUT
{
    float4 pos_WS : SV_POSITION;
};

struct GS_OUTPUT
{
    float4 pos_CS : SV_POSITION;
    nointerpolation uint slice : SV_RenderTargetArrayIndex;
};

//------------------------------------------------------------------------------
// VERTEX SHADER
//------------------------------------------------------------------------------
GS_INPUT vertexShader(VS_INPUT input)
{
    float4x4 transform = float4x4(input.transform_0,
                                  input.transform_1,
                                  input.transform_2,
                                  input.transform_3);

    float4 pos_MS = mul(float4(input.pos, 1.0f), g_mesh_to_model);
    float4 pos_WS = mul(pos_MS, transform);

    GS_INPUT output;
    output.pos_WS = pos_WS;

    return output;
}

//------------------------------------------------------------------------------
// GEOMETRY SHADER
//------------------------------------------------------------------------------
[maxvertexcount(18)]
void geometryShader(triangle GS_INPUT input[3],
                    inout TriangleStream<GS_OUTPUT> output_stream)
{
    // generate 6 triangles for each camera from 1 main triangle
    for (uint face = 0; face != 6; ++face)
    {
        for (uint i = 0; i != 3; ++i)
        {
            GS_OUTPUT output;
            output.slice = face;
            output.pos_CS = mul(input[i].pos_WS, g_proj_view[face]);

            output_stream.Append(output);
        }
        output_stream.RestartStrip();
    }
}

