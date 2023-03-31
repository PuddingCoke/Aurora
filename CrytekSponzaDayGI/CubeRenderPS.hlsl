struct PixelInput
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
};

Texture2D tDiffuse : register(t0);
Texture2D tSpecular : register(t1);
Texture2D<float> shadowTexture : register(t2);

SamplerState wrapSampler : register(s0);
SamplerState clampSampler : register(s1);
SamplerComparisonState shadowSampler : register(s2);

cbuffer Light : register(b2)
{
    float4 lightDir;
    float4 lightColor;
};

cbuffer LightMatrix : register(b3)
{
    matrix lightViewProj;
}

cbuffer ProjMatrices : register(b4)
{
    matrix viewProj[6];
    float3 probeLocation;
    uint probeIndex;
};

float CalShadow(float3 P)
{
    float4 shadowPos = mul(float4(P, 1.0), lightViewProj);
    shadowPos.xy = shadowPos.xy * float2(0.5, -0.5) + 0.5;
    
    float shadow = 0.0;
    const float2 texelSize = 1.0 / float2(4096.0, 4096.0);
    
    [unroll]
    for (int x = -1; x <= 1; x++)
    {
        [unroll]
        for (int y = -1; y <= 1; y++)
        {
            shadow += shadowTexture.SampleCmpLevelZero(shadowSampler, shadowPos.xy + float2(x, y) * texelSize, shadowPos.z);
        }
    }
    
    shadow /= 9.0;
    
    return 1.0 - shadow;
}

struct PixelOuput
{
    float4 color : SV_Target0;
    float dist : SV_Target1;
};

PixelOuput main(PixelInput input)
{
    const float4 baseColor = tDiffuse.Sample(wrapSampler, input.uv);
    
    if (baseColor.a < 0.9)
    {
        discard;
    }
    
    const float specular = tSpecular.Sample(wrapSampler, input.uv).r;
    
    const float3 V = normalize(probeLocation - input.pos);
    
    const float NdotL = max(dot(input.normal, lightDir.xyz), 0.0);
    const float3 diffuseColor = lightColor.rgb * baseColor.rgb * NdotL;
        
    const float3 H = normalize(V + lightDir.xyz);
    const float NdotH = max(dot(input.normal, H), 0.0);
    const float3 specularColor = lightColor.rgb * specular * pow(NdotH, 32.0);
   
    float shadow = CalShadow(input.pos);
    
    float dist = length(probeLocation - input.pos);
    
    float4 color = float4((diffuseColor + specularColor) * CalShadow(input.pos), 1.0);
    
    PixelOuput output;
    output.color = color;
    output.dist = dist;
    
    return output;
}