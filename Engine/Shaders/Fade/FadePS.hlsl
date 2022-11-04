cbuffer DeltaTimes : register(b0)
{
    float deltaTime;
    float v1;
    float v2;
    float v3;
};

cbuffer FadeFactor : register(b1)
{
    float factor;
}

SamplerState linearSampler : register(s0);

Texture2D tTexture : register(t0);

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    float3 color = tTexture.Sample(linearSampler, texCoord).rgb;
    float3 normColor = normalize(color);
    float3 fadeColor = saturate(color - factor * deltaTime);
    return float4(fadeColor, 1.0f);
}