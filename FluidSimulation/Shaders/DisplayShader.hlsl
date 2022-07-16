struct PixelInput
{
    float2 vUv : TEXCOORD0;
    float2 vL : TEXCOORD1;
    float2 vR : TEXCOORD2;
    float2 vT : TEXCOORD3;
    float2 vB : TEXCOORD4;
};

cbuffer DeltaTimes : register(b0)
{
    float deltaTime;
    float v1;
    float v2;
    float v3;
};

cbuffer SimulationConst : register(b1)
{
    float2 velocityTexelSize;
    float2 screenTexelSize;
    float2 sunraysTexelSizeX;
    float2 sunraysTexelSizeY;
    float velocity_dissipation;
    float density_dissipation;
    float value;
    float aspectRatio;
    float curl;
    float radius;
    float weight;
    float v0;
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

Texture2D tTexture : register(t0);
Texture2D tSunrays : register(t1);

float4 main(PixelInput input) : SV_TARGET
{
    float3 c = tTexture.Sample(linearSampler, input.vUv).rgb;
    
    float3 lc = tTexture.Sample(linearSampler, input.vL).rgb;
    float3 rc = tTexture.Sample(linearSampler, input.vR).rgb;
    float3 tc = tTexture.Sample(linearSampler, input.vT).rgb;
    float3 bc = tTexture.Sample(linearSampler, input.vB).rgb;
    
    float dx = length(rc) - length(lc);
    float dy = length(tc) - length(bc);
    
    float3 n = normalize(float3(dx, dy, length(screenTexelSize)));
    float3 l = float3(0.0, 0.0, 1.0);
    
    float diffuse = clamp(dot(n, l) + 0.7, 0.7, 1.0);
    c *= diffuse;
    
    float sunrays = tSunrays.Sample(linearSampler, input.vUv).r;
    c *= sunrays;
    
    float a = max(c.r, max(c.g, c.b));
    return float4(c, a);
}