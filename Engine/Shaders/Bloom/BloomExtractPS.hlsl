Texture2D<float4> originTexture : register(t0);
SamplerState samplerState : register(s0);

cbuffer BloomParam : register(b1)
{
    float exposure;
    float gamma;
    float threshold;
    float intensity;
};

float4 main(float2 texCoord : TEXCOORD) : SV_Target
{
    const float4 color = originTexture.Sample(samplerState, texCoord);
    if (dot(color.rgb, float3(0.2126, 0.7152, 0.0722)) > threshold)
    {
        return color;
    }
    return float4(0.0, 0.0, 0.0, 1.0);
}