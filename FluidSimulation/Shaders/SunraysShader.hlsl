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

Texture2D tTexture : register(t0);

#define ITERATIONS 16

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    float Density = 0.3;
    float Decay = 0.95;
    float Exposure = 0.7;

    float2 coord = texCoord;
    float2 dir = texCoord - 0.5;

    dir *= 1.0 / float(ITERATIONS) * Density;
    float illuminationDecay = 1.0;

    float color = tTexture.Sample(linearSampler, texCoord).a;

    [unroll]
    for (uint i = 0; i < ITERATIONS; i++)
    {
        coord -= dir;
        float col = tTexture.Sample(linearSampler, coord).a;
        color += col * illuminationDecay * weight;
        illuminationDecay *= Decay;
    }
    
    return float4(color * Exposure, 0.0, 0.0, 1.0);
}