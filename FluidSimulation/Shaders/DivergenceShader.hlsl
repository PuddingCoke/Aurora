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

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    float2 texelSize;
    tVelocity.GetDimensions(texelSize.x, texelSize.y);
    texelSize = 1.0 / texelSize;
    
    const float2 vL = texCoord - float2(texelSize.x, 0.0);
    const float2 vR = texCoord + float2(texelSize.x, 0.0);
    const float2 vT = texCoord + float2(0.0, texelSize.y);
    const float2 vB = texCoord - float2(0.0, texelSize.y);
    
    float L = tVelocity.Sample(linearSampler, vL).x;
    float R = tVelocity.Sample(linearSampler, vR).x;
    float T = tVelocity.Sample(linearSampler, vT).y;
    float B = tVelocity.Sample(linearSampler, vB).y;
    
    float2 C = tVelocity.Sample(linearSampler, texCoord).xy;
    
    if (vL.x < 0.0)
    {
        L = -C.x;
    }
    if (vR.x > 1.0)
    {
        R = -C.x;
    }
    if (vT.y > 1.0)
    {
        T = -C.y;
    }
    if (vB.y < 0.0)
    {
        B = -C.y;
    }
    
    float div = 0.5 * (R - L + T - B);
    
    return float4(div, 0.0, 0.0, 1.0);
}