Texture2D gPosition : register(t0);

SamplerState pointSampler : register(s0);

cbuffer ViewInfo : register(b1)
{
    matrix view;
    float4 viewPos;
    matrix prevViewProj;
    matrix viewProj;
    matrix normalMatrix;
};

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    float4 positon = gPosition.Sample(pointSampler, texCoord);
    float4 viewSpacePos = mul(positon, view);
    return viewSpacePos;
}