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
Texture2D tSource : register(t1);


float4 main(float2 texCoord : TEXCOORD) : SV_Target
{
    float2 texelSize;
    tVelocity.GetDimensions(texelSize.x, texelSize.y);
    texelSize = 1.0 / texelSize;
    
    float2 coord = texCoord - deltaTime * tVelocity.Sample(linearSampler, texCoord).xy * texelSize;
    float4 result = tSource.Sample(linearSampler, coord);
    float decay = 1.0 + velocity_dissipation * deltaTime;
    return result / decay;
}