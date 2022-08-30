#include "globals.hlsl"

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
                                                       input.bitangent * -1.0f;

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

// May return direction pointing beneath surface horizon (dot(N, dir) < 0),
// use clampDirToHorizon to fix it.
// R = reflect(V, N)
// cosa - half of solid angle's angular diameter
// L - position of a sphere relative to surface (not normalized)
// radius - radius of light source
float3 approximateClosestSphereDir(float3 R,
                                   float cosa,
                                   float3 L,
                                   float3 L_norm,
                                   float L_length,
                                   float radius)
{
    float RS = dot(R, L_norm);

    bool intersects = (RS >= cosa);
    if (intersects) return R;
    if (RS < 0.0f) return L_norm;

    float3 closest_point_dir = normalize(R * L_length * RS - L);
    return normalize(L + radius * closest_point_dir);
}

// Input dir and NoD is N and NoL in a case of lighting computation
void clampDirToHorizon(inout float3 dir,
                       inout float ND,
                       float3 normal,
                       float minND)
{
    if (ND < minND)
    {
        dir = normalize(dir + (minND - ND) * normal);
        ND = minND;
    }
}

float calculateSolidAngle(float L_length,
                          float radius)
{
    // to avoid black points for roughness -> 0 objects
    // when light source partially in some object
    L_length = max(L_length, radius);

    float R_sqr = L_length * L_length - radius * radius;
    float cosa = sqrt(R_sqr) / L_length;    

    return 2.0f * g_PI * (1.0f - cosa);
}

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
                        float HL,
                        float solid_angle)
{
    float roughness_sqr = material.roughness * material.roughness;
    
    float G = ggxSmith(roughness_sqr, NL, NV);
    float D = ggxTrowbridgeReitz(roughness_sqr, NH);
    float3 F = ggxSchlick(HL, material.fresnel);

    // clamp NDF to avoid light reflection being brighter than a light source
    float D_norm = min(1.0f, solid_angle * D / (4.0f * NV));
    
    return D_norm * F * G;
}

float3 calculateDirectionalLights(Material material,
                                  float3 N,
                                  float3 V)
{
    float3 color = float3(0.0f, 0.0f, 0.0f);
    
    for (uint i = 0; i != g_dir_lights_count; ++i)
    {
        float3 L = -normalize(g_dir_lights[i].direction);

        float3 H = normalize(L + V);        
        float NL = clamp(dot(N, L), 0.0f, 1.0f);
        float NV = clamp(dot(N, V), 0.0f, 1.0f);
        float NH = clamp(dot(N, H), 0.0f, 1.0f);
        float HL = clamp(dot(H, L), 0.0f, 1.0f);

        float3 diffuse = LambertBRDF(material, NL);
        float3 specular = CookTorranceBRDF(material, NL, NV, NH, HL, g_dir_lights[i].solid_angle);
        float3 color_i = (diffuse * g_dir_lights[i].solid_angle + specular) * g_dir_lights[i].radiance;
        
        color += color_i;
    }

    return color;
}

float3 calculatePointLights(Material material,
                            float3 N,
                            float3 GN,
                            float3 V,
                            float3 pos_WS)
{
    float3 color = float3(0.0f, 0.0f, 0.0f);
    
    for (uint i = 0; i != g_point_lights_count; ++i)
    {
        float3 L = g_point_lights[i].position - pos_WS;
        float3 L_norm = normalize(L);

        float3 H = normalize(L_norm + V);
        float NL = clamp(dot(N, L_norm), 0.0f, 1.0f);
        float NV = clamp(dot(N, V), 0.0f, 1.0f);

        // angular diameter of solid angle
        float sina = g_point_lights[i].radius / length(L);
        float cosa = sqrt(1.0f - sina * sina);

        // to avoid illumination of regions of the surface that
        // should be self-shadowed (when light source on the back side of surface)
        float GNL = dot(GN, L_norm);
        float height_micro = NL * length(L); // distance: surface - light source
        float height_macro = GNL * length(L);

        float fading_micro = saturate((height_micro + g_point_lights[i].radius) /
                                      (2.0f * g_point_lights[i].radius));
        float fading_macro = saturate((height_macro + g_point_lights[i].radius) /
                                      (2.0f * g_point_lights[i].radius));
        NL = max(NL, fading_micro * sina);
        
        // take into light source size in specular part
        float3 L2 = approximateClosestSphereDir(reflect(-V, N),
                                                cosa,
                                                L,
                                                L_norm,
                                                length(L),
                                                g_point_lights[i].radius);
        clampDirToHorizon(L2, NL, N, 0.001f);        

        float3 H2 = normalize(L2 + V);
        float NL2 = clamp(dot(N, L2), 0.0f, 1.0f);
        float NH2 = clamp(dot(N, H2), 0.0f, 1.0f);
        float HL2 = clamp(dot(H, L2), 0.0f, 1.0f);

        float solid_angle = calculateSolidAngle(length(L),
                                                g_point_lights[i].radius);
   
        float3 diffuse = LambertBRDF(material, NL);
        float3 specular = CookTorranceBRDF(material, NL2, NV, NH2, HL2, solid_angle);
        float3 color_i = (diffuse * solid_angle + specular) * g_point_lights[i].radiance;

        color += color_i * fading_micro * fading_macro;
    }

    return color;
}

float4 fragmentShader(PS_INPUT input) : SV_TARGET
{
    float3x3 TBN = float3x3(input.tangent,
                            input.bitangent,
                            input.normal);
    
    Material material;
    material.fresnel = float3(g_F0_dielectric, g_F0_dielectric, g_F0_dielectric);

    if (g_has_albedo_texture)
    {
        material.albedo = g_albedo.Sample(g_sampler, input.uv).rgb;
        
        // texture sRGB -> linear (delegated to DDSTextureLoader)
        // material.albedo = pow(material.albedo, g_gamma);
    }
    else material.albedo = g_albedo_default;
        
    if (g_has_roughness_texture)
    {
        material.roughness = g_roughness.Sample(g_sampler, input.uv).r;
    }
    else material.roughness = g_roughness_default;

    // perception roughness -> roughness
    material.roughness *= material.roughness;
    
    if (g_has_metalness_texture)
        material.metalness = g_metalness.Sample(g_sampler, input.uv).r;
    else material.metalness = g_metalness_default;

    // use albedo as F0 for metals
    material.fresnel = lerp(material.fresnel, material.albedo, material.metalness);

    float3 GN = input.normal; // geometry normal
    GN = normalize(mul(float4(GN, 0.0f), g_mesh_to_model).xyz);
    GN = normalize(mul(float4(GN, 0.0f), input.transform).xyz);
    
    float3 N; // texture normal
    if (g_has_normal_map)
    {
        N = g_normal.Sample(g_sampler, input.uv).rgb;
        N = N * 2.0f - 1.0f; // [0; 1] -> [-1; 1]
        N = normalize(mul(N, TBN));
    }
    else N = input.normal;
    N = normalize(mul(float4(N, 0.0f), g_mesh_to_model).xyz);
    N = normalize(mul(float4(N, 0.0f), input.transform).xyz);
    
    float3 V = normalize(g_camera_position - input.pos_WS);
    
    float3 color = float3(0.0f, 0.0f, 0.0f);
    color += calculateDirectionalLights(material, N, V);
    color += calculatePointLights(material, N, GN, V, input.pos_WS);
    
    return float4(color, 1.0f);
}

