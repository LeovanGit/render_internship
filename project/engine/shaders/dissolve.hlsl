#include "globals.hlsl"
#include "lighting.hlsl"
#include "math.hlsl"

cbuffer PerMesh : register(b1)
{
    row_major float4x4 g_mesh_to_model;

    bool g_has_albedo_texture;
    bool g_has_roughness_texture;
    bool g_has_metalness_texture;
    bool g_has_normal_map;
    bool g_is_directx_style_normal_map;
    float3 padding_0;
    
    float3 g_albedo_default;
    float g_roughness_default;
    float g_metalness_default;
    float3 padding_1;
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

    float spawn_time : SPAWN_TIME;
    float animation_time : ANIMATION_TIME;
};

struct PS_INPUT
{
    float4 pos_CS : SV_POSITION;
    float3 pos_WS : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    float4x4 transform : TRANSFORM;

    float spawn_time : SPAWN_TIME;
    float animation_time : ANIMATION_TIME;
};

Texture2D g_albedo : register(t0);
Texture2D g_roughness : register(t1);
Texture2D g_metalness : register(t2);
Texture2D g_normal : register(t3);

Texture2D g_dissolve : register(t13);

static const float4 g_EMISSIVE_COLOR = float4(30.0f, 0.0f, 0.0f, 1.0f);
static const float g_EMISSIVE_THRESHOLD = 0.04f;

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
    output.pos_WS = pos_WS.xyz;
    
    output.uv = input.uv;
    
    output.normal = input.normal;
    output.tangent = input.tangent;
    output.bitangent = g_is_directx_style_normal_map ? input.bitangent :
                                                      -input.bitangent;

    output.transform = transform;
    
    output.spawn_time = input.spawn_time;
    output.animation_time = input.animation_time;

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

float4 fragmentShader(PS_INPUT input,
                      bool is_front_face : SV_IsFrontFace) : SV_TARGET
{
    float alpha = g_dissolve.Sample(g_wrap_sampler, input.uv).r;
    float threshold = (g_time - input.spawn_time) / input.animation_time;
    
    if (alpha > threshold)
    {        
        float delta = alpha - threshold;
        if (delta < g_EMISSIVE_THRESHOLD)
        {
            // [0; g_EMISSIVE_THRESHOLD] -> [0; 1]
            delta *= (1.0f / g_EMISSIVE_THRESHOLD);

            // convert range using power func to get dull edges of emissive border
            // size of dull edges depends on power
            // attention: only even powers!!!
            delta = -pow((2.0f * delta - 1.0f), 2.0f) + 1.0f;

            float4 g_emissive_color_norm = float4(g_EMISSIVE_COLOR.rgb /
                                                  max3(g_EMISSIVE_COLOR), 1.0f);

            return lerp(g_emissive_color_norm,
                        g_EMISSIVE_COLOR,
                        delta);
        }
        else return float4(0.0f, 0.0f, 0.0f, 0.0f);
    }

    // geometry normal
    float3 GN = normalize(is_front_face ? input.normal : -input.normal);
    
    float3x3 TBN = float3x3(input.tangent,
                            input.bitangent,
                            GN);

    GN = normalize(mul(float4(GN, 0.0f), g_mesh_to_model).xyz);
    GN = normalize(mul(float4(GN, 0.0f), input.transform).xyz);
    
    Material material;

    // conversion from sRGB to linear by raising to the power of 2.2
    // is delegated to DDSTextureLoader
    material.albedo = g_has_albedo_texture ? g_albedo.Sample(g_wrap_sampler, input.uv).rgb :
                                             g_albedo_default;

    material.roughness = g_has_roughness_texture ? g_roughness.Sample(g_wrap_sampler, input.uv).r :
                                                   g_roughness_default;
    // perception roughness -> roughness
    // material.roughness *= material.roughness;

    material.metalness = g_has_metalness_texture ? g_metalness.Sample(g_wrap_sampler, input.uv).r :
                                                   g_metalness_default;
        
    // use albedo as F0 for metals
    material.fresnel = lerp(g_F0_DIELECTRIC, material.albedo, material.metalness);

    // texture normal
    float3 N;
    if (g_has_normal_map)
    {
        N = g_normal.Sample(g_wrap_sampler, input.uv).rgb;
        N = 2.0f * N - 1.0f; // [0; 1] -> [-1; 1]
        N = normalize(mul(N, TBN));
        N = normalize(mul(float4(N, 0.0f), g_mesh_to_model).xyz);
        N = normalize(mul(float4(N, 0.0f), input.transform).xyz);
    }
    else N = GN;
    
    float3 V = normalize(g_camera_position - input.pos_WS);
    
    float3 color = calculateLighting(material.albedo,
                                     material.roughness,
                                     material.metalness,
                                     material.fresnel,
                                     N,
                                     GN,
                                     V,
                                     input.pos_WS);

    return float4(color, 1.0f);
}
