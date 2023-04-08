#include"Common.hlsli"

struct PixelInput
{
    float3 dir : NORMAL;
    uint probeIndex : PROBEINDEX;
};

Texture2DArray<float3> irradianceCoeff : register(t0);
Texture2DArray<float2> depthOctahedralMap : register(t1);

SamplerState linearClampSampler : register(s0);

float4 main(PixelInput input) : SV_TARGET
{
    return float4(GetIrradiance(input.dir, input.probeIndex, irradianceCoeff), 1.0);
}