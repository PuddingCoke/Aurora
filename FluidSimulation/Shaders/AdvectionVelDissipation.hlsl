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
Texture2D tSource : register(t1);


float4 main(float2 texCoord : TEXCOORD) : SV_Target
{
    float2 coord = texCoord - deltaTime * tVelocity.Sample(linearSampler, texCoord).xy * simTexelSize;
    float4 result = tSource.Sample(linearSampler, coord);
    float decay = 1.0 + velocity_dissipation * deltaTime;
    return result / decay;
}