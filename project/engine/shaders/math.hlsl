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
