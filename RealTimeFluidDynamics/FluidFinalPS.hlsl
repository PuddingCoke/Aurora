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

Texture2D colorTex : register(t0);

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    float3 color = colorTex.Sample(linearSampler, texCoord).rgb;
    
    const float2 vL = texCoord - float2(screenTexelSize.x, 0.0);
    const float2 vR = texCoord + float2(screenTexelSize.x, 0.0);
    const float2 vT = texCoord + float2(0.0, screenTexelSize.y);
    const float2 vB = texCoord - float2(0.0, screenTexelSize.y);
    
    float3 lc = colorTex.Sample(linearSampler, vL).rgb;
    float3 rc = colorTex.Sample(linearSampler, vR).rgb;
    float3 tc = colorTex.Sample(linearSampler, vT).rgb;
    float3 bc = colorTex.Sample(linearSampler, vB).rgb;
    
    float dx = length(rc) - length(lc);
    float dy = length(tc) - length(bc);
    
    float3 n = normalize(float3(dx, dy, length(screenTexelSize)));
    float3 l = float3(0.0, 0.0, 1.0);
    
    float diffuse = clamp(dot(n, l) + 0.7, 0.7, 1.0);
    color *= diffuse;
    
    float a = max(color.r, max(color.g, color.b));
    return float4(color, a);
}