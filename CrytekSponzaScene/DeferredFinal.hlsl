Texture2D tPositionDepth : register(t0);
Texture2D tNormalSpecular : register(t1);
Texture2D tAlbedo : register(t2);

SamplerState linearSampler : register(s0);

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    return float4(tPositionDepth.Sample(linearSampler, texCoord).rgb, 1.0);
}