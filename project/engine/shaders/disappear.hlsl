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

    uint model_id : MODEL_ID;
    float model_box_diameter : BOX_DIAMETER;
    float spawn_time : SPAWN_TIME;
    float animation_time : ANIMATION_TIME;
    float3 sphere_origin : SPHERE_ORIGIN;
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
    uint model_id : MODEL_ID;
    float model_box_diameter : BOX_DIAMETER;
    float spawn_time : SPAWN_TIME;
    float animation_time : ANIMATION_TIME;
    float3 sphere_origin : SPHERE_ORIGIN;
};

struct PS_OUTPUT
{
    float4 normals : SV_TARGET0;
    float3 albedo : SV_TARGET1;
    float2 roughness_metalness : SV_TARGET2;
    float4 emissive_ao : SV_TARGET3;
    uint model_id : SV_TARGET4;
};

Texture2D g_albedo : register(t0);
Texture2D g_roughness : register(t1);
Texture2D g_metalness : register(t2);
Texture2D g_normal : register(t3);
Texture2D g_noise : register(t4);

static const float4 g_EMISSIVE_COLOR = float4(30.0f, 0.0f, 0.0f, 1.0f);
static const float g_EMISSIVE_THRESHOLD = 0.2f;

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
    output.model_id = input.model_id;
    output.model_box_diameter = input.model_box_diameter;
    output.spawn_time = input.spawn_time;
    output.animation_time = input.animation_time;
    output.sphere_origin = input.sphere_origin;

    return output;
}

//------------------------------------------------------------------------------
// FRAGMENT SHADER
//------------------------------------------------------------------------------
PS_OUTPUT fragmentShader(PS_INPUT input,
                         bool is_front_face: SV_IsFrontFace)
{    
    PS_OUTPUT output;
    output.emissive_ao = float4(0.0f, 0.0f, 0.0f, 1.0f);

    float time = g_time - input.spawn_time;
    float sphere_radius = input.model_box_diameter * time / input.animation_time;
    float dist = length(input.pos_WS - input.sphere_origin);
    
    if (dist < sphere_radius)
    {
        if (dist > (sphere_radius - g_EMISSIVE_THRESHOLD))
        {
            output.emissive_ao = g_EMISSIVE_COLOR *
                                 g_noise.Sample(g_wrap_sampler, input.uv).r;
            output.emissive_ao.a = 1.0f;
        }
        else
        {
            discard;
            return output;   
        }       
    }
    
    // geometry normal
    float3 GN = normalize(is_front_face ? input.normal : -input.normal);
    
    float3x3 TBN = float3x3(input.tangent,
                            input.bitangent,
                            GN);

    GN = normalize(mul(float4(GN, 0.0f), g_mesh_to_model).xyz);
    GN = normalize(mul(float4(GN, 0.0f), input.transform).xyz);
    output.normals.ba = packOctahedron(GN);
    
    // conversion from sRGB to linear by raising to the power of 2.2
    // is delegated to DDSTextureLoader
    output.albedo = g_has_albedo_texture ?
                        g_albedo.Sample(g_wrap_sampler, input.uv).rgb :
                        g_albedo_default;

    output.roughness_metalness.r = g_has_roughness_texture ?
                                       g_roughness.Sample(g_wrap_sampler, input.uv).r :
                                       g_roughness_default;
    
    output.roughness_metalness.g = g_has_metalness_texture ?
                                       g_metalness.Sample(g_wrap_sampler, input.uv).r :
                                       g_metalness_default;

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
    output.normals.rg = packOctahedron(N);

    output.model_id = input.model_id;
    
    return output;
}

