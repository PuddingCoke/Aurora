Texture2D objTexture : register(t0);
SamplerState objSamplerState : register(s0);

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    return float4(objTexture.Sample(objSamplerState, texCoord).rrr, 1.0);
}