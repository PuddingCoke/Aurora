Texture2D<float4> tTexture : register(t0);
Texture2D<float4> lensDirtTexture : register(t1);
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
    float3 hdrColor = tTexture.Sample(samplerState, texCoord).rgb;
    float3 dirtColor = lensDirtTexture.Sample(samplerState, texCoord).rgb;
    hdrColor += hdrColor * dirtColor;
    float3 result = 1.0 - exp(-hdrColor * exposure);
    result = pow(result, 1.0 / gamma);
    return float4(result, 1.0);
}