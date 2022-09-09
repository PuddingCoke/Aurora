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

Texture2D tTarget : register(t0);

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    float2 p = texCoord - point0.xy;
    p.x *= aspectRatio;
    float3 splat = exp(-dot(p, p) / radius) * color0;
    float3 base = tTarget.Sample(linearSampler, texCoord).xyz;
    
    return float4(base + splat, 1.0);
}