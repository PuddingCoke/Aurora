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

Texture2D pressureTex : register(t0);
Texture2D divergenceTex : register(t1);

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{    
    const float L = pressureTex.Sample(pointSampler, texCoord - float2(simTexelSize.x, 0.0)).x;
    const float R = pressureTex.Sample(pointSampler, texCoord + float2(simTexelSize.x, 0.0)).x;
    const float T = pressureTex.Sample(pointSampler, texCoord + float2(0.0, simTexelSize.y)).x;
    const float B = pressureTex.Sample(pointSampler, texCoord - float2(0.0, simTexelSize.y)).x;
    const float C = pressureTex.Sample(pointSampler, texCoord).x;
    
    float divergence = divergenceTex.Sample(pointSampler, texCoord).x;
    
    float pressure = (L + R + B + T - divergence) * 0.25;
    
    return float4(pressure, 0.0, 0.0, 1.0);
}