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
    const float2 vL = texCoord - float2(simTexelSize.x, 0.0);
    const float2 vR = texCoord + float2(simTexelSize.x, 0.0);
    const float2 vT = texCoord + float2(0.0, simTexelSize.y);
    const float2 vB = texCoord - float2(0.0, simTexelSize.y);
    
    float L = velocityTex.Sample(linearSampler, texCoord - float2(simTexelSize.x, 0.0)).x;
    float R = velocityTex.Sample(linearSampler, texCoord + float2(simTexelSize.x, 0.0)).x;
    float T = velocityTex.Sample(linearSampler, texCoord + float2(0.0, simTexelSize.y)).y;
    float B = velocityTex.Sample(linearSampler, texCoord - float2(0.0, simTexelSize.y)).y;
    float2 C = velocityTex.Sample(linearSampler, texCoord).xy;
    
    //boundary
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
    
    float divergence = 0.5 * (R - L + T - B);
    
    return float4(divergence, 0.0, 0.0, 1.0);
}