Texture2D tex : register(t0);

SamplerState linearSampler : register(s0);

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    return float4(tex.Sample(linearSampler, texCoord).rg, 0.0, 1.0);
}