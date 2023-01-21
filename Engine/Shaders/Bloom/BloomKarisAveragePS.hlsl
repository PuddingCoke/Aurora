cbuffer BloomParam : register(b1)
{
    float exposure;
    float gamma;
    float threshold;
    float intensity;
};

Texture2D<float4> sourceTexture : register(t0);
SamplerState linearSampler : register(s0);

float RGBToLuminance(float3 color)
{
    return dot(color, float3(0.2126, 0.7152, 0.0722));
}

float KarisAverage(float3 col)
{
    float luma = RGBToLuminance(pow(col, 1.0 / gamma)) * 0.25;
    return 1.0 / (1.0 + luma);
}

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    float2 texelSize;
    sourceTexture.GetDimensions(texelSize.x, texelSize.y);
    texelSize = 1.0 / texelSize;
    
    const float3 a = sourceTexture.Sample(linearSampler, float2(texCoord.x - 2 * texelSize.x, texCoord.y + 2 * texelSize.y)).rgb;
    const float3 b = sourceTexture.Sample(linearSampler, float2(texCoord.x, texCoord.y + 2 * texelSize.y)).rgb;
    const float3 c = sourceTexture.Sample(linearSampler, float2(texCoord.x + 2 * texelSize.x, texCoord.y + 2 * texelSize.y)).rgb;

    const float3 d = sourceTexture.Sample(linearSampler, float2(texCoord.x - 2 * texelSize.x, texCoord.y)).rgb;
    const float3 e = sourceTexture.Sample(linearSampler, float2(texCoord.x, texCoord.y)).rgb;
    const float3 f = sourceTexture.Sample(linearSampler, float2(texCoord.x + 2 * texelSize.x, texCoord.y)).rgb;

    const float3 g = sourceTexture.Sample(linearSampler, float2(texCoord.x - 2 * texelSize.x, texCoord.y - 2 * texelSize.y)).rgb;
    const float3 h = sourceTexture.Sample(linearSampler, float2(texCoord.x, texCoord.y - 2 * texelSize.y)).rgb;
    const float3 i = sourceTexture.Sample(linearSampler, float2(texCoord.x + 2 * texelSize.x, texCoord.y - 2 * texelSize.y)).rgb;

    const float3 j = sourceTexture.Sample(linearSampler, float2(texCoord.x - texelSize.x, texCoord.y + texelSize.y)).rgb;
    const float3 k = sourceTexture.Sample(linearSampler, float2(texCoord.x + texelSize.x, texCoord.y + texelSize.y)).rgb;
    const float3 l = sourceTexture.Sample(linearSampler, float2(texCoord.x - texelSize.x, texCoord.y - texelSize.y)).rgb;
    const float3 m = sourceTexture.Sample(linearSampler, float2(texCoord.x + texelSize.x, texCoord.y - texelSize.y)).rgb;
    
    float3 groups[5];
    groups[0] = (a + b + d + e) * (0.125f / 4.0f);
    groups[1] = (b + c + e + f) * (0.125f / 4.0f);
    groups[2] = (d + e + g + h) * (0.125f / 4.0f);
    groups[3] = (e + f + h + i) * (0.125f / 4.0f);
    groups[4] = (j + k + l + m) * (0.5f / 4.0f);
    groups[0] *= KarisAverage(groups[0]);
    groups[1] *= KarisAverage(groups[1]);
    groups[2] *= KarisAverage(groups[2]);
    groups[3] *= KarisAverage(groups[3]);
    groups[4] *= KarisAverage(groups[4]);
    
    float3 color = groups[0] + groups[1] + groups[2] + groups[3] + groups[4];
    
    return float4(color, 1.0);
}