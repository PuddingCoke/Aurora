Texture2D objTexture : TEXTURE : register(t0);
SamplerState objSamplerState : SAMPLER : register(s0);

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    float4 color = objTexture.Sample(objSamplerState, texCoord);
    return float4(color.rgb, color.a * 0.5);
}