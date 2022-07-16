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

Texture2D tVelocity : register(t0);
Texture2D tCurl : register(t1);

float4 main(PixelInput input) : SV_TARGET
{
    float L = tCurl.Sample(pointSampler, input.vL).x;
    float R = tCurl.Sample(pointSampler, input.vR).x;
    float T = tCurl.Sample(pointSampler, input.vT).x;
    float B = tCurl.Sample(pointSampler, input.vB).x;
    float C = tCurl.Sample(pointSampler, input.vUv).x;
    
    float2 force = 0.5 * float2(abs(T) - abs(B), abs(R) - abs(L));
    force /= length(force) + 0.0001;
    force *= curl * C;
    force.y *= -1.0;
    
    float2 vel = tVelocity.Sample(linearSampler, input.vUv).xy;
    
    return float4(vel + force * deltaTime, 0.0, 1.0);
}