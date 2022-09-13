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

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    float4 sum = tTexture.Sample(linearSampler, texCoord) * 0.29411764;
    
    const float offset = 1.33333333;
    
    const float2 vL = texCoord - float2(sunTexelSize.x, 0.0) * offset;
    const float2 vR = texCoord + float2(sunTexelSize.x, 0.0) * offset;
    
    sum += tTexture.Sample(linearSampler, vL) * 0.35294117;
    sum += tTexture.Sample(linearSampler, vR) * 0.35294117;
    return sum;
}