#include "globals.hlsl"
#include "math.hlsl"

struct VS_INPUT
{
    float3 posWS : POSITION;
    float2 size : SIZE;
    float3 albedo : ALBEDO;

    float4 transform_0 : TRANSFORM0;
    float4 transform_1 : TRANSFORM1;
    float4 transform_2 : TRANSFORM2;
    float4 transform_3 : TRANSFORM3;
};

struct PS_INPUT
{
    float4 posCS : SV_POSITION;
    float3 albedo : ALBEDO;
};

struct PS_OUTPUT
{
    float4 normals : SV_TARGET0;
    float3 albedo : SV_TARGET1;
    float2 roughness_metalness : SV_TARGET2;
    float4 emissive_ao : SV_TARGET3;
};

Texture2D<float3> g_decal_normals : register(t0);

//------------------------------------------------------------------------------
// VERTEX SHADER
//------------------------------------------------------------------------------
PS_INPUT vertexShader(uint vertex_index: SV_VERTEXID,
                      VS_INPUT input)
{
    float2 half_size = input.size.xy / 2.0f;
    float half_depth = input.size.x / 2.0f;

    // cubeMS (render only back faces)
    float3 vertexMS[36] =
    {
        // front
        float3(-half_size.x, -half_size.y, +half_depth),
        float3(-half_size.x, +half_size.y, +half_depth),
        float3(+half_size.x, +half_size.y, +half_depth),

        float3(+half_size.x, +half_size.y, +half_depth),
        float3(+half_size.x, -half_size.y, +half_depth),
        float3(-half_size.x, -half_size.y, +half_depth),

        // back
        float3(+half_size.x, +half_size.y, -half_depth),
        float3(-half_size.x, +half_size.y, -half_depth),
        float3(-half_size.x, -half_size.y, -half_depth),

        float3(-half_size.x, -half_size.y, -half_depth),
        float3(+half_size.x, -half_size.y, -half_depth),
        float3(+half_size.x, +half_size.y, -half_depth),

        // left
        float3(-half_size.x, -half_size.y, -half_depth),
        float3(-half_size.x, +half_size.y, -half_depth),
        float3(-half_size.x, +half_size.y, +half_depth),

        float3(-half_size.x, +half_size.y, +half_depth),
        float3(-half_size.x, -half_size.y, +half_depth),
        float3(-half_size.x, -half_size.y, -half_depth),

        // right
        float3(+half_size.x, +half_size.y, +half_depth),
        float3(+half_size.x, +half_size.y, -half_depth),
        float3(+half_size.x, -half_size.y, -half_depth),

        float3(+half_size.x, -half_size.y, -half_depth),
        float3(+half_size.x, -half_size.y, +half_depth),
        float3(+half_size.x, +half_size.y, +half_depth),

        // top
        float3(-half_size.x, +half_size.y, +half_depth),
        float3(-half_size.x, +half_size.y, -half_depth),
        float3(+half_size.x, +half_size.y, -half_depth),

        float3(-half_size.x, +half_size.y, +half_depth),
        float3(+half_size.x, +half_size.y, -half_depth),
        float3(+half_size.x, +half_size.y, +half_depth),

        // down
        float3(+half_size.x, -half_size.y, -half_depth),
        float3(-half_size.x, -half_size.y, -half_depth),
        float3(-half_size.x, -half_size.y, +half_depth),

        float3(+half_size.x, -half_size.y, +half_depth),
        float3(+half_size.x, -half_size.y, -half_depth),
        float3(-half_size.x, -half_size.y, +half_depth),
    };

    float4x4 transform = float4x4(input.transform_0,
                                  input.transform_1,
                                  input.transform_2,
                                  input.transform_3);

    //float4x4 trans = translate(input.posWS);
    float4 posWS = mul(float4(vertexMS[vertex_index], 1.0f), transform);
    
    PS_INPUT output;

    output.posCS = mul(posWS, g_proj_view);
    output.albedo = input.albedo;

    return output;
}

//------------------------------------------------------------------------------
// FRAGMENT SHADER
//------------------------------------------------------------------------------
PS_OUTPUT fragmentShader(PS_INPUT input)
{
    PS_OUTPUT output;

    output.normals.rg = packOctahedron(float3(0.0f, 0.0f, -1.0f));
    output.normals.ba = packOctahedron(float3(0.0f, 0.0f, -1.0f));
    output.albedo = float4(input.albedo, 1.0f);
    output.roughness_metalness = float2(0.1f, 0.0f);
    output.emissive_ao = float4(0.0f, 0.0f, 0.0f, 1.0f);

    return output;
}
