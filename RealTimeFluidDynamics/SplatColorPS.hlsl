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

Texture2D<float4> colorTex : register(t0);

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    float2 p = texCoord - pos;
    p.x *= aspectRatio;
    float3 color = exp(-dot(p, p) / splatRadius) * splatColor.rgb;
    float3 curColor = colorTex.Sample(linearSampler, texCoord).rgb;
    return float4(curColor + color, 1.0);
}