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

SamplerState linearSampler : register(s0);
SamplerState pointSampler : register(s1);

Texture2D tPressure : register(t0);
Texture2D tDivergence : register(t1);

float4 main(PixelInput input) : SV_TARGET
{
    float L = tPressure.Sample(pointSampler, input.vL).x;
    float R = tPressure.Sample(pointSampler, input.vR).x;
    float T = tPressure.Sample(pointSampler, input.vT).x;
    float B = tPressure.Sample(pointSampler, input.vB).x;
    float C = tPressure.Sample(pointSampler, input.vUv).x;
    
    float divergence = tDivergence.Sample(pointSampler, input.vUv).x;
    
    float pressure = (L + R + B + T - divergence) * 0.25;
    
    return float4(pressure, 0.0, 0.0, 1.0);
}