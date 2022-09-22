Texture2D depthTexture : register(t0);

SamplerState linearSampler : register(s0);

float4 main(float2 texCoord:TEXCOORD) : SV_TARGET
{
    return float4(depthTexture.Sample(linearSampler, texCoord).rrr, 1.0);

}