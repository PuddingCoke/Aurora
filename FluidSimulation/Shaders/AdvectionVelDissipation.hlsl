struct PixelInput
{
    float2 vUv : TEXCOORD0;
    float2 vL : TEXCOORD1;
    float2 vR : TEXCOORD2;
    float2 vT : TEXCOORD3;
    float2 vB : TEXCOORD4;
};

cbuffer DeltaTimes : register(b0)
{
    float deltaTime;
    float v1;
    float v2;
    float v3;
};

cbuffer SimulationConst : register(b1)
{
    float2 velocityTexelSize;
    float2 screenTexelSize;
    float2 sunraysTexelSizeX;
    float2 sunraysTexelSizeY;
    float velocity_dissipation;
    float density_dissipation;
    float value;
    float aspectRatio;
    float curl;
    float radius;
    float weight;
    float v0;
}

cbuffer SimulationDynamic : register(b2)
{
    float3 color0;
    float padding0;
    float3 color1;
    float padding1;
    float2 point0;
    float2 padding2;
}

Texture2D tVelocity : register(t0);
Texture2D tSource : register(t1);

SamplerState linearSampler : register(s0);
SamplerState pointSampler : register(s1);

float4 main(PixelInput input) : SV_Target
{
    float2 coord = input.vUv - deltaTime * tVelocity.Sample(linearSampler, input.vUv).xy * velocityTexelSize;
    float4 result = tSource.Sample(linearSampler, coord);
    float decay = 1.0 + velocity_dissipation * deltaTime;
    return result / decay;
}