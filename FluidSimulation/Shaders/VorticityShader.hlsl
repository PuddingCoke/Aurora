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

Texture2D tVelocity : register(t0);
Texture2D tCurl : register(t1);

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    const float2 vL = texCoord - float2(simTexelSize.x, 0.0);
    const float2 vR = texCoord + float2(simTexelSize.x, 0.0);
    const float2 vT = texCoord + float2(0.0, simTexelSize.y);
    const float2 vB = texCoord - float2(0.0, simTexelSize.y);
    
    float L = tCurl.Sample(pointSampler, vL).x;
    float R = tCurl.Sample(pointSampler, vR).x;
    float T = tCurl.Sample(pointSampler, vT).x;
    float B = tCurl.Sample(pointSampler, vB).x;
    float C = tCurl.Sample(pointSampler, texCoord).x;
    
    float2 force = 0.5 * float2(abs(T) - abs(B), abs(R) - abs(L));
    force *= 1.0 / (length(force) + 0.0001) * curl * C;
    force.y = -force.y;
    
    float2 vel = tVelocity.Sample(linearSampler, texCoord).xy;
    
    return float4(vel + force * deltaTime, 0.0, 1.0);
}