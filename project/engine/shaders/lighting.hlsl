#ifndef LIGHTING_HLSL
#define LIGHTING_HLSL

Texture2D<float2> g_reflectance : register(t6);
TextureCube g_irradiance : register(t7);
TextureCube g_reflection : register(t8);

TextureCubeArray<float4> g_shadow_maps : register(t9);

static const float g_DEPTH_OFFSET = 0.005f;
static const float g_TRANSMITTANCE_POWER = 32.0f;

uint calculateCubeMapFaceIndex(float3 dir)
{
    // compare positive axes
    float max_value = max(abs(dir.x), max(abs(dir.y), abs(dir.z)));
    uint index = abs(dir.x) == max_value ? 0 : (abs(dir.y) == max_value ? 2 : 4);

    // compare positive with negative axes and
    // increment index if negative is dominates
    return index + (asuint(dir[index / 2]) >> 31);
}

float calculateShadowCoefficient(float3 pos_WS,
                                 float3 N,
                                 float3 L,
                                 float3 light_pos_WS,
                                 uint cubemap_index)
{    
    float4x4 face_proj_view =
        g_light_proj_view[6 * cubemap_index + calculateCubeMapFaceIndex(-L)];

    pos_WS += L * g_DEPTH_OFFSET; // affects only comp_depth, but not sample_dir

    // only 3 and 4 rows are used (.z and .w):
    float4 pos_CS = mul(float4(pos_WS, 1.0f), face_proj_view);
    float comp_depth = pos_CS.z / pos_CS.w;

    float linear_depth = pos_CS.w;
    float normal_offset = 2.0f * linear_depth / g_shadow_map_size;

    pos_WS += N * normal_offset; // affects only sample_dir, but not comp_depth
    float3 sample_dir = pos_WS - light_pos_WS;

    float visibility = g_shadow_maps.SampleCmp(g_comparison_sampler,
                                               float4(sample_dir, cubemap_index),
                                               comp_depth);
    
    return 1.0f - visibility;
}

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

    float sina = radius / L_length;
    float cosa = sqrt(1.0f - sina * sina);

    return 2.0f * g_PI * (1.0f - cosa);
}

float3 calculateEnvironment(float3 albedo,
                            float roughness,
                            float metalness,
                            float3 fresnel,
                            float3 N,
                            float3 V)
{
    float NV = max(dot(N, V), 0.001f);
    
    float3 diffuse = albedo * (1.0f - metalness) * g_irradiance.SampleLevel(g_clamp_sampler, N, 0);

    float2 reflectanceLUT = g_reflectance.SampleLevel(g_clamp_sampler, float2(roughness, 1.0f - NV), 0);
    float3 reflectance = reflectanceLUT.x * fresnel + reflectanceLUT.y;
    int mip = roughness * g_reflection_mips_count;
    float3 specular = reflectance * g_reflection.SampleLevel(g_clamp_sampler, reflect(-V, N), mip);

    return diffuse + specular;
}

// ignore specular IBL
float3 calculateEnvironment(float3 albedo,
                            float metalness,
                            float3 N)
{   
    return albedo * (1.0f - metalness) * g_irradiance.SampleLevel(g_clamp_sampler, N, 0);
}

float3 calculateSurfaceLightTransmission(float3 posWS,
                                         float3 N,
                                         float3 transmittance_color)
{
    float3 transmittion = 0.0f;
    
    for (uint i = 0; i != g_POINT_LIGHTS_COUNT; ++i)
    {
        float3 L = g_point_lights[i].position - posWS;
        float3 L_norm = normalize(L);

        float solid_angle = calculateSolidAngle(length(L),
                                                g_point_lights[i].radius);
        float NL = dot(N, L_norm);

        // if light source is behind the surface
        if (NL < 0.0f)
        {
            float shadow = calculateShadowCoefficient(posWS,
                                                      N,
                                                      L,
                                                      g_point_lights[i].position,
                                                      i);

            transmittion += g_point_lights[i].radiance *
                            transmittance_color *
                            solid_angle *
                            pow(-NL, g_TRANSMITTANCE_POWER);
        }
    }

    for (uint i = 0; i != g_DIR_LIGHTS_COUNT; ++i)
    {
        float3 L = -normalize(g_dir_lights[i].direction);
        float NL = dot(N, L);

        // if light source is behind the surface
        if (NL < 0.0f) transmittion += g_dir_lights[i].radiance *
                                       transmittance_color *
                                       pow(-NL, g_TRANSMITTANCE_POWER) *
                                       g_dir_lights[i].solid_angle;
    }
    
    return transmittion;
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

float3 LambertBRDF(float3 albedo,
                   float metalness,
                   float3 fresnel,
                   float NL)
{
    float3 F = ggxSchlick(NL, fresnel);

    return albedo * (1.0f - metalness) * (1.0f - F) * NL / g_PI;
}

float3 CookTorranceBRDF(float roughness,
                        float3 fresnel,
                        float NL,
                        float NV,
                        float NH,
                        float HL,
                        float solid_angle)
{
    float roughness_sqr = roughness * roughness;
    
    float G = ggxSmith(roughness_sqr, NL, NV);
    float D = ggxTrowbridgeReitz(roughness_sqr, NH);
    float3 F = ggxSchlick(HL, fresnel);

    // clamp NDF to avoid light reflection being brighter than a light source
    float D_norm = min(1.0f, solid_angle * D / (4.0f * NV));
    
    return D_norm * F * G;
}

float3 calculateDirectionalLights(float3 albedo,
                                  float roughness,
                                  float metalness,
                                  float3 fresnel,
                                  float3 N,
                                  float3 V)
{
    float3 color = 0.0f;
    
    for (uint i = 0; i != g_DIR_LIGHTS_COUNT; ++i)
    {
        float3 L = -normalize(g_dir_lights[i].direction);

        float3 H = normalize(L + V);        
        float NL = max(dot(N, L), 0.001f);
        float NV = max(dot(N, V), 0.001f);
        float NH = max(dot(N, H), 0.001f);
        float HL = max(dot(H, L), 0.001f);

        float3 diffuse = LambertBRDF(albedo, metalness, fresnel, NL);
        float3 specular = CookTorranceBRDF(roughness,
                                           fresnel,
                                           NL,
                                           NV,
                                           NH,
                                           HL,
                                           g_dir_lights[i].solid_angle);
        float3 color_i = (diffuse * g_dir_lights[i].solid_angle + specular) * g_dir_lights[i].radiance;
        
        color += color_i;
    }

    return color;
}

float3 calculatePointLights(float3 albedo,
                            float roughness,
                            float metalness,
                            float3 fresnel,
                            float3 N,
                            float3 GN,
                            float3 V,
                            float3 pos_WS)
{
    float3 color = float3(0.0f, 0.0f, 0.0f);
    
    for (uint i = 0; i != g_POINT_LIGHTS_COUNT; ++i)
    {        
        float3 L = g_point_lights[i].position - pos_WS;
        float3 L_norm = normalize(L);

        float3 H = normalize(L_norm + V);
        float NL = max(dot(N, L_norm), 0.001f);
        float NV = max(dot(N, V), 0.001f);

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
        float NL2 = max(dot(N, L2), 0.001f);
        float NH2 = max(dot(N, H2), 0.001f);
        float HL2 = max(dot(H, L2), 0.001f);

        float solid_angle = calculateSolidAngle(length(L),
                                                g_point_lights[i].radius);
   
        float3 diffuse = LambertBRDF(albedo, metalness, fresnel, NL);
        float3 specular = CookTorranceBRDF(roughness,
                                           fresnel,
                                           NL2,
                                           NV,
                                           NH2,
                                           HL2,
                                           solid_angle);
        float3 color_i = (diffuse * solid_angle + specular) * g_point_lights[i].radiance;

        float shadow = calculateShadowCoefficient(pos_WS,
                                                  N,
                                                  L,
                                                  g_point_lights[i].position,
                                                  i);
        
        color += color_i * fading_micro * fading_macro * shadow;
    }

    return color;
}

float3 calculateLighting(float3 albedo,
                         float roughness,
                         float metalness,
                         float3 fresnel,
                         float3 N,
                         float3 GN,
                         float3 V,
                         float3 pos_WS)
{
    float3 color = 0.0f;
    color += calculateDirectionalLights(albedo,
                                        roughness,
                                        metalness,
                                        fresnel,
                                        N,
                                        V);
    
    color += calculatePointLights(albedo,
                                  roughness,
                                  metalness,
                                  fresnel,
                                  N,
                                  GN,
                                  V,
                                  pos_WS);

    color += calculateEnvironment(albedo,
                                  roughness,
                                  metalness,
                                  fresnel,
                                  N,
                                  V);
    
    return color;
}

//------------------------------------------------------------------------------
// LIGHTMAPS LIGHTING
//------------------------------------------------------------------------------
float3 calculateDirectionalLights(float3 right,
                                  float3 up,
                                  float3 normal,
                                  float3 lightmap_RLT,
                                  float3 lightmap_BotBF)
{
    float3 color = 0.0f;

    for (uint i = 0; i != g_DIR_LIGHTS_COUNT; ++i)
    {
        float3 L_norm = -normalize(g_dir_lights[i].direction);

        float3 color_i = g_dir_lights[i].radiance *
                         g_dir_lights[i].solid_angle;

        float RL = dot(right, L_norm);
        float UL = dot(up, L_norm);
        float NL = dot(normal, L_norm);

        color_i *= ((RL > 0 ? lightmap_RLT.r : lightmap_RLT.g) * abs(RL) +
                    (UL > 0 ? lightmap_RLT.b : lightmap_BotBF.r) * abs(UL) +
                    (NL > 0 ? lightmap_BotBF.b : lightmap_BotBF.g) * abs(NL));

        color += color_i;
    }

    return color;
}

float3 calculatePointLights(float3 pos_WS,
                            float3 right,
                            float3 up,
                            float3 normal,
                            float3 lightmap_RLT,
                            float3 lightmap_BotBF)
{
    float3 color = 0.0f;
    
    for (uint i = 0; i != g_POINT_LIGHTS_COUNT; ++i)
    {
        float3 L = g_point_lights[i].position - pos_WS;
        float3 L_norm = normalize(L);
        
        float3 color_i = g_point_lights[i].radiance *
                         calculateSolidAngle(length(L), g_point_lights[i].radius);

        float RL = dot(right, L_norm);
        float UL = dot(up, L_norm);
        float NL = dot(normal, L_norm);

        color_i = color_i * (RL > 0 ? lightmap_RLT.r : lightmap_RLT.g) * abs(RL) +
                  color_i * (UL > 0 ? lightmap_RLT.b : lightmap_BotBF.r) * abs(UL) +
                  color_i * (NL > 0 ? lightmap_BotBF.b : lightmap_BotBF.g) * abs(NL);
        
        color += color_i;
    }

    return color;
}

float3 calculateLighting(float3 pos_WS,
                         float3 right, // normalized particle basis
                         float3 up,
                         float3 normal,
                         float3 lightmap_RLT,
                         float3 lightmap_BotBF)
{
    float3 color = 0.0f;

    color += calculateDirectionalLights(right,
                                        up,
                                        normal,
                                        lightmap_RLT,
                                        lightmap_BotBF);

    color += calculatePointLights(pos_WS,
                                  right,
                                  up,
                                  normal,
                                  lightmap_RLT,
                                  lightmap_BotBF);

    return color;
}

#endif
