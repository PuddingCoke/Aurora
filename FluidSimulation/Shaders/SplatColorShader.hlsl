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

cbuffer SimulationDynamic : register(b2)
{
    float2 mousePos;
    float2 mouseDelta;
    float4 color;
}

SamplerState linearSampler : register(s0);
SamplerState pointSampler : register(s1);

Texture2D tTarget : register(t0);

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    float2 p = texCoord - mousePos;
    p.x *= aspectRatio;
    float3 splat = exp(-dot(p, p) / radius) * color.rgb;
    float3 base = tTarget.Sample(linearSampler, texCoord).xyz;
    
    return float4(base + splat, 1.0);
}