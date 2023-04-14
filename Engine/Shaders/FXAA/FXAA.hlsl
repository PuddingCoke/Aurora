#define FXAA_PC 1
#define FXAA_HLSL_5 1
#define FXAA_GREEN_AS_LUMA 0
#define FXAA_QUALITY__PRESET 39

#include"FXAAHeader.hlsli"

Texture2D<float4> originTexture : register(t0); //a channel should be luminance

SamplerState clampSampler : register(s0);

cbuffer FXAAParam:register(b1)
{
    float fxaaQualityRcpFrame; // unused
    float fxaaQualitySubpix;
    float fxaaQualityEdgeThreshold;
    float fxaaQualityEdgeThresholdMin;
};

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    uint dx, dy;
    originTexture.GetDimensions(dx, dy);
    float2 rcpro = rcp(float2(dx, dy));
    
    FxaaTex t;
    t.tex = originTexture;
    t.smpl = clampSampler;

    return FxaaPixelShader(texCoord, 0, t, t, t, rcpro, 0, 0, 0, fxaaQualitySubpix, fxaaQualityEdgeThreshold, fxaaQualityEdgeThresholdMin, 0, 0, 0, 0);
}