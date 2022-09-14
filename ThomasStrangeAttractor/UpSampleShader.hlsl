Texture2D tTexture : register(t0);

SamplerState linearSampler : register(s0);

cbuffer PixelOffset : register(b1)
{
    float offset;
    float3 v0;
}

cbuffer PixelHalf : register(b2)
{
    float2 halfpixel;
    float2 v1;
};

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    float4 sum = tTexture.Sample(linearSampler, texCoord + float2(-halfpixel.x * 2.0, 0.0) * offset);
    sum += tTexture.Sample(linearSampler, texCoord + float2(-halfpixel.x, halfpixel.y) * offset) * 2.0;
    sum += tTexture.Sample(linearSampler, texCoord + float2(0.0, halfpixel.y * 2.0) * offset);
    sum += tTexture.Sample(linearSampler, texCoord + float2(halfpixel.x, halfpixel.y) * offset) * 2.0;
    sum += tTexture.Sample(linearSampler, texCoord + float2(halfpixel.x * 2.0, 0.0) * offset);
    sum += tTexture.Sample(linearSampler, texCoord + float2(halfpixel.x, -halfpixel.y) * offset) * 2.0;
    sum += tTexture.Sample(linearSampler, texCoord + float2(0.0, -halfpixel.y * 2.0) * offset);
    sum += tTexture.Sample(linearSampler, texCoord + float2(-halfpixel.x, -halfpixel.y) * offset) * 2.0;
    return float4(sum / 12.0);
}