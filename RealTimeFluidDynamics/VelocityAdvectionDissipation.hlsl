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

Texture2D<float2> velocityTex : register(t0);

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    const float2 coord = texCoord - dt * velocityTex.Sample(linearSampler, texCoord) * simTexelSize;
    const float2 result = velocityTex.Sample(linearSampler, coord);
    const float decay = 1.0 + velocityDissipationSpeed * dt;
    return float4(result / decay, 0.0, 1.0);
}