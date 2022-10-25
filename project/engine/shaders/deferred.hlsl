#include "globals.hlsl"
#include "lighting.hlsl"
#include "math.hlsl"

Texture2D<float4> g_normals : register(t0);
Texture2D<float3> g_albedo : register(t1);
Texture2D<float2> g_roughness_metalness : register(t2);
Texture2D<float4> g_emissive_ao : register(t3);
Texture2D<float> g_depth : register(t5);

struct PS_INPUT
{
    float4 posCS : SV_POSITION;
};

//------------------------------------------------------------------------------
// VERTEX SHADER
//------------------------------------------------------------------------------
PS_INPUT vertexShader(uint vertex_index: SV_VERTEXID)
{
    // generate fullscreen triangle in clip space
    float2 vertexCS[3] = {{-1.0f, -1.0f},
                          {-1.0f,  3.0f},
                          { 3.0f, -1.0f}};

    PS_INPUT output;
    output.posCS = float4(vertexCS[vertex_index], 1.0f, 1.0f);
    
    return output;
}

//------------------------------------------------------------------------------
// FRAGMENT SHADER
//------------------------------------------------------------------------------
float4 fragmentShader(PS_INPUT input) : SV_TARGET
{
    float4 normals = g_normals.Load(int3(input.posCS.xy, 0));
    float3 N = unpackOctahedron(normals.rg);    
    float3 GN = unpackOctahedron(normals.ba);
    
    float3 albedo = g_albedo.Load(int3(input.posCS.xy, 0));
    float2 RM = g_roughness_metalness.Load(int3(input.posCS.xy, 0));
    float4 emissive_ao = g_emissive_ao.Load(int3(input.posCS.xy, 0));
    float depth = g_depth.Load(int3(input.posCS.xy, 0)).r;

    float2 posCS = float2((input.posCS.x / g_screen_size.x) * 2.0f - 1.0f,
                          1.0f - 2.0f * (input.posCS.y / g_screen_size.y));
    
    float4 posWS_h = mul(float4(posCS, depth, 1.0f), g_proj_view_inv);
    float3 posWS = posWS_h.xyz / posWS_h.w;

    float3 V = normalize(g_camera_position - posWS);

    // use albedo as F0 for metals
    float3 fresnel = lerp(g_F0_DIELECTRIC, albedo, RM.g);

    float3 color = calculateLighting(albedo,
                                     RM.r,
                                     RM.g,
                                     fresnel,
                                     N,
                                     GN,
                                     V,
                                     posWS);

    color *= emissive_ao.a;
    color += emissive_ao.rgb;
    
    return float4(color, 1.0f);
}
