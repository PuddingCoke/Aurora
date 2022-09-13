cbuffer DeltaTimes : register(b0)
{
    float deltaTime;
    float v1;
    float v2;
    float v3;
};

cbuffer SimulationConst : register(b1)
{
    float2 screenTexelSize;
    float2 simTexelSize;
    float2 sunTexelSize;
    float velocity_dissipation;
    float density_dissipation;
    float value;
    float aspectRatio;
    float curl;
    float radius;
    float weight;
    float3 v0;
}

SamplerState linearSampler : register(s0);
SamplerState pointSampler : register(s1);

Texture2D tPressure : register(t0);
Texture2D tVelocity : register(t1);

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    const float2 vL = texCoord - float2(simTexelSize.x, 0.0);
    const float2 vR = texCoord + float2(simTexelSize.x, 0.0);
    const float2 vT = texCoord + float2(0.0, simTexelSize.y);
    const float2 vB = texCoord - float2(0.0, simTexelSize.y);
    
    float L = tPressure.Sample(pointSampler, vL).x;
    float R = tPressure.Sample(pointSampler, vR).x;
    float T = tPressure.Sample(pointSampler, vT).x;
    float B = tPressure.Sample(pointSampler, vB).x;
    
    float2 velocity = tVelocity.Sample(linearSampler, texCoord).xy;
    velocity.xy -= float2(R - L, T - B);
    
    return float4(velocity, 0.0, 1.0);
}