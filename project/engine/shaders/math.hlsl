float max3(float3 v)
{
    return max(v.x, max(v.y, max(v.z, 1.0f)));
}

// angle in radians
row_major float4x4 rotateY(float angle)
{
    float sina, cosa;
    sincos(angle, sina, cosa);
    
    float4x4 rotate = float4x4(cosa, 0.0f, sina, 0.0f,
                               0.0f, 1.0f, 0.0f, 0.0f,
                               -sina, 0.0f, cosa, 0.0f,
                               0.0f, 0.0f, 0.0f, 1.0f);

    return rotate;
}

row_major float4x4 translate(float3 position)
{
    float4x4 translate = float4x4(1.0f, 0.0f, 0.0f, 0.0f,
                                  0.0f, 1.0f, 0.0f, 0.0f,
                                  0.0f, 0.0f, 1.0f, 0.0f,
                                  position,         1.0f);
    
    return translate;
}

float2 nonZeroSign(float2 v)
{
    return float2(v.x >= 0.0f ? 1.0f : -1.0f,
                  v.y >= 0.0f ? 1.0f : -1.0f);
}

float2 packOctahedron(float3 v)
{
    float2 p = v.xy / (abs(v.x) + abs(v.y) + abs(v.z));
    return v.z <= 0.0f ? (float2(1.0f, 1.0f) - abs(p.yx)) * nonZeroSign(p) : p;
}

float3 unpackOctahedron(float2 oct)
{
    float3 v = float3(oct, 1.0f - abs(oct.x) - abs(oct.y));
    if (v.z < 0) v.xy = (float2(1.0f, 1.0f) - abs(v.yx)) * nonZeroSign(v.xy);
    return normalize(v);
}
