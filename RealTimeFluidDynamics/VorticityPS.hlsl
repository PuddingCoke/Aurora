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
Texture2D<float> curlTex : register(t1);

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    const float L = curlTex.Sample(pointSampler, texCoord - float2(simTexelSize.x, 0.0));
    const float R = curlTex.Sample(pointSampler, texCoord + float2(simTexelSize.x, 0.0));
    const float T = curlTex.Sample(pointSampler, texCoord + float2(0.0, simTexelSize.y));
    const float B = curlTex.Sample(pointSampler, texCoord - float2(0.0, simTexelSize.y));
    const float C = curlTex.Sample(pointSampler, texCoord);
    
    float2 force = 0.5 * float2(abs(B) - abs(T), abs(R) - abs(L));
    force *= 1.0 / (length(force) + 0.0001) * curlIntensity * C;
    //force.y = -force.y;
    
    const float2 curVel = velocityTex.Sample(linearSampler, texCoord);
    
    return float4(curVel + force * dt, 0.0, 1.0);
}