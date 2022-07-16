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

Texture2D tVelocity;

float4 main(PixelInput input) : SV_TARGET
{
    float L = tVelocity.Sample(linearSampler, input.vL).x;
    float R = tVelocity.Sample(linearSampler, input.vR).x;
    float T = tVelocity.Sample(linearSampler, input.vT).y;
    float B = tVelocity.Sample(linearSampler, input.vB).y;
    
    float2 C = tVelocity.Sample(linearSampler, input.vUv).xy;
    
    if (input.vL.x < 0.0)
    {
        L = -C.x;
    }
    if (input.vR.x > 1.0)
    {
        R = -C.x;
    }
    if (input.vT.y > 1.0)
    {
        T = -C.y;
    }
    if (input.vB.y < 0.0)
    {
        B = -C.y;
    }
    
    float div = 0.5 * (R - L + T - B);
    
    return float4(div, 0.0, 0.0, 1.0);
}