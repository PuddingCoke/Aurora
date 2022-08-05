struct PixelInput
{
    float2 texCoord : TEXCOORD;
};

cbuffer DeltaTimes : register(b0)
{
    float deltaTime;
    float sTime;
    float2 v0;
}

SamplerState linearSampler : register(s0);

Texture2D tTexture : register(t0);

float4 main(PixelInput input) : SV_TARGET
{
    return float4(0.0, 0.0, 1.0, 1.0);
    //return tTexture.Sample(linearSampler, input.texCoord);
}