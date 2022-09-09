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
    float velocity_dissipation;
    float density_dissipation;
    float value;
    float aspectRatio;
    float curl;
    float radius;
    float weight;
    float3 v0;
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

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    float2 texelSize;
    tVelocity.GetDimensions(texelSize.x, texelSize.y);
    texelSize = 1.0 / texelSize;
    
    const float2 vL = texCoord - float2(texelSize.x, 0.0);
    const float2 vR = texCoord + float2(texelSize.x, 0.0);
    const float2 vT = texCoord + float2(0.0, texelSize.y);
    const float2 vB = texCoord - float2(0.0, texelSize.y);
    
    float L = tCurl.Sample(pointSampler, vL).x;
    float R = tCurl.Sample(pointSampler, vR).x;
    float T = tCurl.Sample(pointSampler, vT).x;
    float B = tCurl.Sample(pointSampler, vB).x;
    float C = tCurl.Sample(pointSampler, texCoord).x;
    
    float2 force = 0.5 * float2(abs(T) - abs(B), abs(R) - abs(L));
    force /= length(force) + 0.0001;
    force *= curl * C;
    force.y *= -1.0;
    
    float2 vel = tVelocity.Sample(linearSampler, texCoord).xy;
    
    return float4(vel + force * deltaTime, 0.0, 1.0);
}