SamplerState linearSampler : register(s0);

Texture2D envTexture : register(t0);

static const float2 invAtan = float2(0.1591, 0.3183);

float2 SampleSphjericalMap(float3 v)
{
    float2 uv = float2(atan2(v.x, v.z), asin(-v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

float4 main(float3 position : POSITION) : SV_TARGET
{
    float2 uv = SampleSphjericalMap(normalize(position));
    return envTexture.Sample(linearSampler, uv);
}