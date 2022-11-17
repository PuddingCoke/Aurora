TextureCube skyCube : register(t0);

SamplerState linearSampler : register(s0);

cbuffer DeltaTimes : register(b0)
{
    float dt;
    float sTime;
    float2 v0;
}

float4 main(float3 position : POSITION) : SV_TARGET
{
    float3 envColor = skyCube.SampleLevel(linearSampler, position, 0.0).rgb;
    return float4(envColor, 1.0);
}