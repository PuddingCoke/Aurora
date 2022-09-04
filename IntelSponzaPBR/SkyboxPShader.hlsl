SamplerState linearSampler : register(s0);

TextureCube envCube : register(t0);

float4 main(float3 position : POSITION) : SV_TARGET
{
    float3 envColor = envCube.Sample(linearSampler, position).rgb;
    return float4(envColor, 1.0);
}