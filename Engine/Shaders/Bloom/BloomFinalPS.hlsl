Texture2D<float4> originTexture : register(t0);
Texture2D<float4> bloomTexture : register(t1);
Texture2D<float4> lensDirtTexture : register(t2);
SamplerState samplerState : register(s0);

cbuffer BloomParam : register(b1)
{
    float exposure;
    float gamma;
    float threshold;
    float intensity;
};

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    float3 originColor = originTexture.Sample(samplerState, texCoord).rgb;
    float3 dirtColor = lensDirtTexture.Sample(samplerState, texCoord).rgb;
    float3 bloomColor = bloomTexture.Sample(samplerState, texCoord).rgb * (1.0 + dirtColor);
    float3 result = lerp(originColor, bloomColor, intensity);
    result = 1.0 - exp(-result * exposure);
    result = pow(result, 1.0 / gamma);
    return float4(result, 1.0);
}