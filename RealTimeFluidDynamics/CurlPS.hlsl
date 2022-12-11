cbuffer DeltaTime : register(b0)
{
    float dt;
    float sTime;
    uint uintSeed;
    float floatSeed;
}

cbuffer SimulationParam : register(b1)
{
    float2 screenTexelSize;
    float2 simTexelSize;
    float colorDissipationSpeed;
    float velocityDissipationSpeed;
    float pressureDissipationSpeed;
    float curlIntensity;
    float aspectRatio;
    float splatRadius;
    float2 v0;
}

cbuffer SimulationDelta : register(b2)
{
    float2 pos;
    float2 posDelta;
    float4 splatColor;
}

SamplerState pointSampler : register(s0);
SamplerState linearSampler : register(s1);

Texture2D velocityTex : register(t0);

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    float L = velocityTex.Sample(linearSampler, texCoord - float2(simTexelSize.x, 0.0)).y;
    float R = velocityTex.Sample(linearSampler, texCoord + float2(simTexelSize.x, 0.0)).y;
    float T = velocityTex.Sample(linearSampler, texCoord + float2(0.0, simTexelSize.y)).x;
    float B = velocityTex.Sample(linearSampler, texCoord - float2(0.0, simTexelSize.y)).x;
    
    float vorticity = 0.5 * (R - L - T + B);
    
    return float4(vorticity, 0.0, 0.0, 1.0);
}