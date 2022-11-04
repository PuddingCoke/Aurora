struct PixelInput
{
    float2 texCoord : TEXCOORD;
};

Texture2D tex : register(t0);
SamplerState samplerState : register(s0);

float4 main(PixelInput input) : SV_TARGET
{
    return tex.Sample(samplerState, input.texCoord);
}