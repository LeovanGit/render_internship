#include "globals.hlsl"

cbuffer PerMesh : register(b1)
{
    row_major float4x4 g_mesh_to_model;
    
    bool g_has_roughness_texture;
    bool g_has_metalness_texture;
    bool g_has_normal_map;

    float g_roughness_default;
    float g_metalness_default;
    float3 padding_0;
}

struct VS_INPUT
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    
    float4 transform_0 : TRANSFORM0;
    float4 transform_1 : TRANSFORM1;
    float4 transform_2 : TRANSFORM2;
    float4 transform_3 : TRANSFORM3;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3x3 TBN : TBN;
    float4x4 transform : TRANSFORM;
};

Texture2D g_albedo : register(t0);
Texture2D g_roughness : register(t1);
Texture2D g_metalness : register(t2);
Texture2D g_normal : register(t3);

//------------------------------------------------------------------------------
// VERTEX SHADER
//------------------------------------------------------------------------------
PS_INPUT vertexShader(VS_INPUT input)
{
    float4x4 transform = float4x4(input.transform_0,
                                  input.transform_1,
                                  input.transform_2,
                                  input.transform_3);

    float4 pos = mul(float4(input.pos, 1.0f), g_mesh_to_model);
    pos = mul(pos, transform);
    pos = mul(pos, g_proj_view);

    float3x3 TBN = float3x3(input.tangent,
                            input.bitangent,
                            input.normal);
    
    PS_INPUT output;
    output.uv = input.uv;
    output.pos = pos;
    output.normal = input.normal;
    output.TBN = TBN;
    output.transform = transform;

    return output;
}

//------------------------------------------------------------------------------
// FRAGMENT SHADER
//------------------------------------------------------------------------------
struct Material
{
    float3 albedo;
    float roughness;
    float metalness;
    float3 fresnel;
};

// G
float ggxSmith(float roughness_sqr,
               float NL,
               float NV)
{
    float NL_sqr = NL * NL;
    float NV_sqr = NV * NV;

    // fast Smith from Filament
    return 2.0f / (sqrt(1 + roughness_sqr * (1 - NV_sqr) / NV_sqr) +
                   sqrt(1 + roughness_sqr * (1 - NL_sqr) / NL_sqr));

}

// D
float ggxTrowbridgeReitz(float roughness_sqr,
                         float NH)
{
    float NH_sqr = NH * NH;
    float denom = NH_sqr * (roughness_sqr - 1.0f) + 1.0f;
    float D = roughness_sqr / (g_PI  * denom * denom);

    return D;
}

// F
float3 ggxSchlick(float cosTheta,
                  float3 F0)
{
    return F0 + (1.0f - F0) * pow(1.0f - cosTheta, 5.0f);
}

float3 LambertBRDF(Material material,
                   float NL)
{
    float3 F = ggxSchlick(NL, material.fresnel);

    return material.albedo * (1.0f - material.metalness) * (1.0f - F) * NL / g_PI;
}

float3 CookTorranceBRDF(Material material,
                        float NL,
                        float NV,
                        float NH,
                        float HL)
{
    float roughness_sqr = material.roughness * material.roughness;
    
    float G = ggxSmith(roughness_sqr, NL, NV);
    float D = ggxTrowbridgeReitz(roughness_sqr, NH);
    float3 F = ggxSchlick(HL, material.fresnel);

    // clamp NDF to avoid light reflection being brighter than a light source
    float solid_angle = 1.0f; // tmp
    float D_norm = min(1.0f, solid_angle * D / (4 * NV));
    
    return D_norm * F * G;
}

float3 PBR(Material material,
           float3 N,
           float3 L,
           float3 V)
{
    // tmp light value
    float power = 15.0f;
    float3 light_radiance = float3(power, power, power);
    
    float3 H = normalize(L + V);

    float NL = clamp(dot(N, L), 0.0f, 1.0f);
    float NV = clamp(dot(N, V), 0.0f, 1.0f);
    float NH = clamp(dot(N, H), 0.0f, 1.0f);
    float HL = clamp(dot(H, L), 0.0f, 1.0f);

    float3 diffuse = LambertBRDF(material, NL);
    float3 specular = CookTorranceBRDF(material, NL, NV, NH, HL);

    return (diffuse + specular) * light_radiance;
}

float4 fragmentShader(PS_INPUT input) : SV_TARGET
{
    Material material;
    material.albedo = g_albedo.Sample(g_sampler, input.uv);
    material.fresnel = float3(0.04f, 0.04f, 0.04f);

    // texture sRGB -> linear
    // material.albedo = pow(material.albedo, g_gamma);
    
    if (g_has_roughness_texture)
    {
        material.roughness = g_roughness.Sample(g_sampler, input.uv);

        // perception roughness -> roughness
        material.roughness *= material.roughness;
    }
    else material.roughness = g_roughness_default;
    
    if (g_has_metalness_texture)
        material.metalness = g_metalness.Sample(g_sampler, input.uv);
    else material.metalness = g_metalness_default;

    // use albedo as F0 for metals
    material.fresnel = lerp(material.fresnel, material.albedo, material.metalness);

    float3 N; // fragment normal
    
    if (g_has_normal_map) N = g_normal.Sample(g_sampler, input.uv);
    else N = input.normal;

    N = normalize(N * 2.0f - 1.0f); // [0; 1] -> [-1; 1]
    N = mul(N, input.TBN);
    N = mul(float4(N, 0.0f), input.transform);

    // float3 light_pos = float3(3.0f, -3.0f, -6.0f);
    // float3 L = normalize(light_pos - input.pos.xyz); // tmp light
    float3 L = normalize(-float3(-1.0f, -1.0f, 1.0f)); // tmp light
    float3 V = normalize(g_camera_position - input.pos.xyz);

    return float4(material.albedo, 1.0f);
    
    float3 color = PBR(material, N, L, V);
    
    return float4(color, 1.0f);
}

