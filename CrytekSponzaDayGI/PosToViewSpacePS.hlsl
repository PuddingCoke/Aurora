Texture2D<float4> gPosition : register(t0);

SamplerState wrapSampler : register(s0);
SamplerState clampSampler : register(s1);
SamplerComparisonState shadowSampler : register(s2);

cbuffer ViewMatrix : register(b1)
{
    matrix view;
    float4 viewPos;
    matrix prevViewProj;
    matrix viewProj;
    matrix normalMatrix;
};

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    return mul(gPosition.Sample(clampSampler, texCoord), view);
}