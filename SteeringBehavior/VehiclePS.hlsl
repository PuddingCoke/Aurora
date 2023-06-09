Texture2D arrowTexture : register(t0);

SamplerState linearSampler : register(s0);

struct PixelInput
{
    float2 texCoord : TEXCOORD;
};

float4 main(PixelInput input) : SV_TARGET
{
    return arrowTexture.Sample(linearSampler, input.texCoord);
}