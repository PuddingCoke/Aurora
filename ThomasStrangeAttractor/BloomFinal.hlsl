Texture2D tTexture : register(t0);
SamplerState samplerState : register(s0);

//cbuffer DeltaTimes : register(b1)
//{
//    float exposure;
//    float gamma;
//    float2 v0;
//};

static const float exposure = 0.75;

static const float gamma = 1.2;

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    float3 hdrColor = tTexture.Sample(samplerState, texCoord).rgb;
    float3 result = float3(1.0, 1.0, 1.0) - exp(-hdrColor * exposure);
    result = pow(result, float3(1.0 / gamma, 1.0 / gamma, 1.0 / gamma));
    return float4(result, 1.0);
}