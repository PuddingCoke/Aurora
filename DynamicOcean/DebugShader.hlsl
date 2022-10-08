Texture2D tex : register(t0);

SamplerState linearSampler : register(s0);

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    float height = tex.Sample(linearSampler, texCoord).r;
    return float4(height.rrr, 1.0);
}