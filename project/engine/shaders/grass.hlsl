#include "globals.hlsl"
#include "lighting.hlsl"
#include "math.hlsl"

struct VS_INPUT
{
    float3 posWS : POSITION;
    float2 size : SIZE;
};

struct PS_INPUT
{
    float4 posCS : SV_POSITION;
    float3 posWS : POSITION;
    float2 uv : UV;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
};

struct PS_OUTPUT
{
    float3 normals : SV_TARGET0;
    float3 geometry_normals : SV_TARGET1;
    float3 albedo : SV_TARGET2;
    float2 roughness_metalness : SV_TARGET3;
    float3 emissive : SV_TARGET4;
};

Texture2D<float3> g_grass_albedo : register(t0);
Texture2D<float> g_grass_opacity : register(t1);
Texture2D<float> g_grass_roughness : register(t2);
Texture2D<float3> g_grass_normal : register(t3);
Texture2D<float> g_ambient_occlusion : register(t8);
Texture2D<float3> g_grass_translucency : register(t9);

//------------------------------------------------------------------------------
// VERTEX SHADER
//------------------------------------------------------------------------------
PS_INPUT vertexShader(uint vertex_index: SV_VERTEXID,
                      VS_INPUT input)
{
    float2 half_size = input.size.xy / 2.0f;

    int plane_index = int(vertex_index / 6.0f);

    float angle = (1.0f / g_PLANES_PER_GRASS) * plane_index * g_PI;
    float4x4 rotate = rotateY(angle);
    float4x4 trans = translate(input.posWS);
    
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
    posWS = mul(posWS, trans);

    PS_INPUT output;
    output.posCS = mul(posWS, g_proj_view);
    output.posWS = posWS.xyz;
    output.uv = uv[vertex_index % 6.0f];
    output.normal = mul(float4(0.0f, 0.0f, -1.0f, 0.0f), rotate).xyz;
    output.tangent = mul(float4(1.0f, 0.0f, 0.0f, 0.0f), rotate).xyz;
    output.bitangent = mul(float4(0.0f, 1.0f, 0.0f, 0.0f), rotate).xyz;

    return output;
}

//------------------------------------------------------------------------------
// FRAGMENT SHADER
//------------------------------------------------------------------------------
PS_OUTPUT fragmentShader(PS_INPUT input,
                         bool is_front_face : SV_IsFrontFace) : SV_TARGET
{
    PS_OUTPUT output;
    
    float opacity = g_grass_opacity.Sample(g_wrap_sampler, input.uv);
    if (opacity < 0.01f)
    {
        discard;
        return output;
    }
    
    // geometry normal
    float3 GN = normalize(is_front_face ? input.normal : -input.normal);
    output.geometry_normals = GN;
    
    float3x3 TBN = float3x3(input.tangent,
                            input.bitangent,
                            GN);
    
    
    output.albedo = g_grass_albedo.Sample(g_wrap_sampler, input.uv);
    output.roughness_metalness.r = g_grass_roughness.Sample(g_wrap_sampler, input.uv);
    output.roughness_metalness.g = 0.0f;
    
    // texture normal
    float3 N = g_grass_normal.Sample(g_wrap_sampler, input.uv).rgb;
    N = 2.0f * N - 1.0f; // [0; 1] -> [-1; 1]
    N = normalize(mul(N, TBN));
    output.normals = N;

    float3 transmittance_color = g_grass_translucency.Sample(g_wrap_sampler, input.uv);
    output.emissive = transmittance_color;
    
    // float ao = g_ambient_occlusion.Sample(g_wrap_sampler, input.uv); ???
    
    //return float4(color * ao, 1.0f);
    return output;
}
