struct PixelOutput
{
    float4 color : SV_TARGET0;
    float4 brightColor : SV_TARGET1;
};

Texture2D<float4> tTexture : register(t0);
SamplerState samplerState : register(s0);

cbuffer BloomParam : register(b1)
{
    float exposure;
    float gamma;
    float threshold;
    float intensity;
};

PixelOutput main(float2 texCoord : TEXCOORD)
{
    PixelOutput output;
    const float4 color = tTexture.Sample(samplerState, texCoord);
    output.color = color;
    output.brightColor = float4(0.0, 0.0, 0.0, 1.0);
    if (dot(color.rgb, float3(0.2126, 0.7152, 0.0722)) > threshold)
    {
        output.brightColor = color;
    }
    return output;
}