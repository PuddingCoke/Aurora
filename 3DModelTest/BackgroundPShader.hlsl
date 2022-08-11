SamplerState linearSampler : register(s0);

TextureCube envCube : register(t0);

float4 main(float3 position : POSITION) : SV_TARGET
{
    float3 envColor = envCube.Sample(linearSampler, position).rgb;
    envColor = envColor / (envColor + float3(1.0, 1.0, 1.0));
    envColor = pow(envColor, float3(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2));
    return float4(envColor, 1.0);
}