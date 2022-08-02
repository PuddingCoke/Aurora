SamplerState linearSampler : register(s0);

Texture2D accTexture : register(t0);

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    float3 color = accTexture.Sample(linearSampler, texCoord).rgb;
    color *= 40.0;
    return float4(color, 1.0);
}