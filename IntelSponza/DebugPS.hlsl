Texture2D tex : register(t0);

SamplerState samplerState : register(s0);

float4 main(float2 texCoord:TEXCOORD) : SV_TARGET
{
    float4 color = tex.Sample(samplerState, texCoord);
    return float4(color.ggg, 1.0);
}