#include "globals.hlsl"
#include "math.hlsl"

struct VS_INPUT
{
    float3 posWS : POSITION;
    float2 size : SIZE;
    float3 albedo : ALBEDO;

    float4 model_0 : MODEL0;
    float4 model_1 : MODEL1;
    float4 model_2 : MODEL2;
    float4 model_3 : MODEL3;

    float4 model_inv_0 : MODEL_INV0;
    float4 model_inv_1 : MODEL_INV1;
    float4 model_inv_2 : MODEL_INV2;
    float4 model_inv_3 : MODEL_INV3;

    uint model_id : MODEL_ID;
};

struct PS_INPUT
{
    float4 posCS : SV_POSITION;
    nointerpolation row_major float4x4 model : MODEL;
    nointerpolation row_major float4x4 model_inv : MODEL_INV;
    float3 albedo : ALBEDO;
    float3 half_size : SIZE;
    uint model_id : MODEL_ID;
};

struct PS_OUTPUT
{
    float4 normals : SV_TARGET0;
    float3 albedo : SV_TARGET1;
    float2 roughness_metalness : SV_TARGET2;
    float4 emissive_ao : SV_TARGET3;
};

Texture2D<float4> g_decal_normals : register(t0);
Texture2D<float> g_depth : register(t1);
Texture2D<float4> g_normals : register(t2);
Texture2D<uint> g_model_ids : register(t3);

static const float g_DECAL_ROUGHNESS = 0.25f;
static const float g_DECAL_METALNESS = 0.0f;

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
    
    // decal space -> ws
    float4x4 model_matrix = float4x4(input.model_0,
                                     input.model_1,
                                     input.model_2,
                                     input.model_3);

    float4x4 model_matrix_inv = float4x4(input.model_inv_0,
                                         input.model_inv_1,
                                         input.model_inv_2,
                                         input.model_inv_3);

    float4 posWS = mul(float4(vertexMS[vertex_index], 1.0f), model_matrix);
    
    PS_INPUT output;

    output.posCS = mul(posWS, g_proj_view);
    output.model = model_matrix;
    output.model_inv = model_matrix_inv;
    output.albedo = input.albedo;
    output.half_size = float3(half_size, half_depth);
    output.model_id = input.model_id;

    return output;
}

//------------------------------------------------------------------------------
// FRAGMENT SHADER
//------------------------------------------------------------------------------
PS_OUTPUT fragmentShader(PS_INPUT input)
{
    PS_OUTPUT output;

    if (input.model_id != g_model_ids.Load(int3(input.posCS.xy, 0)).r)
    {
        discard;
        return output;
    }
    
    float depth = g_depth.Load(int3(input.posCS.xy, 0)).r;

    float2 posCS = float2((input.posCS.x / g_screen_size.x) * 2.0f - 1.0f,
                          1.0f - 2.0f * (input.posCS.y / g_screen_size.y));
    
    float4 posWS = mul(float4(posCS, depth, 1.0f), g_proj_view_inv);

    float4 posMS_h = mul(posWS, input.model_inv);
    float3 posMS = posMS_h.xyz / posMS_h.w;

    if (abs(posMS.x) > input.half_size.x ||
        abs(posMS.y) > input.half_size.y ||
        abs(posMS.z) > input.half_size.z)
    {
        discard;
        return output;
    }

    float2 uv = ((posMS.xy / input.half_size.xy) + 1.0f) / 2.0f;
    uv.y = 1.0f - uv.y;
    float4 normals = g_decal_normals.Sample(g_wrap_sampler, uv);

    if (normals.w < 0.55f)
    {
        discard;
        return output;
    }

    float4 scene_normals = g_normals.Load(int3(input.posCS.xy, 0));    
    float3 scene_N = unpackOctahedron(scene_normals.rg);
    float3 scene_GN = unpackOctahedron(scene_normals.ba);

    float3 decal_right = input.model[0].xyz;
    float3 tangent = normalize(decal_right - scene_N * dot(scene_N, decal_right));
    float3x3 TBN = float3x3(tangent,
                            cross(scene_N, tangent),
                            scene_GN);
    
    float3 N = normals.xyz;
    N = 2.0f * N - 1.0f;
    N = normalize(mul(N, TBN));
    
    output.normals.rg = packOctahedron(normalize(N + scene_N));
    output.normals.ba = packOctahedron(normalize(N + scene_N));
    output.albedo = float4(input.albedo, 1.0f);    
    output.roughness_metalness = float2(g_DECAL_ROUGHNESS, g_DECAL_METALNESS);
    output.emissive_ao = float4(0.0f, 0.0f, 0.0f, 1.0f);

    return output;
}
