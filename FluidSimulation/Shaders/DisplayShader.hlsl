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
Texture2D tSunrays : register(t1);

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    float3 c = tTexture.Sample(linearSampler, texCoord).rgb;
    
    const float2 vL = texCoord - float2(screenTexelSize.x, 0.0);
    const float2 vR = texCoord + float2(screenTexelSize.x, 0.0);
    const float2 vT = texCoord + float2(0.0, screenTexelSize.y);
    const float2 vB = texCoord - float2(0.0, screenTexelSize.y);
    
    float3 lc = tTexture.Sample(linearSampler, vL).rgb;
    float3 rc = tTexture.Sample(linearSampler, vR).rgb;
    float3 tc = tTexture.Sample(linearSampler, vT).rgb;
    float3 bc = tTexture.Sample(linearSampler, vB).rgb;
    
    float dx = length(rc) - length(lc);
    float dy = length(tc) - length(bc);
    
    float3 n = normalize(float3(dx, dy, length(screenTexelSize)));
    float3 l = float3(0.0, 0.0, 1.0);
    
    float diffuse = clamp(dot(n, l) + 0.7, 0.7, 1.0);
    c *= diffuse;
    
    float sunrays = tSunrays.Sample(linearSampler, texCoord).r;
    c *= sunrays;
    
    float a = max(c.r, max(c.g, c.b));
    return float4(c, a);
}