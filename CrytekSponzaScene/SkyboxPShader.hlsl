SamplerState linearSampler : register(s1);

TextureCube skyCube : register(t0);

float4 main(float3 position : POSITION) : SV_TARGET
{
    float3 envColor = skyCube.SampleLevel(linearSampler, position, 0.0).rgb;
    return float4(envColor, 1.0);
}