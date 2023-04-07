#include"Common.hlsli"

Texture2D gPosition : register(t0);
Texture2D gNormalSpecular : register(t1);
Texture2D gBaseColor : register(t2);
Texture2D ssaoTexture : register(t3);
Texture2D<float> shadowTexture : register(t4);
Texture2DArray<float3> irradianceCoeff : register(t5);
Texture2DArray<float2> depthOctahedralMap : register(t6);

SamplerState wrapSampler : register(s0);
SamplerState clampSampler : register(s1);
SamplerComparisonState shadowSampler : register(s2);

cbuffer DeltaTimes : register(b0)
{
    float deltaTime;
    float sTime;
    float2 v0;
}

cbuffer ViewMatrix : register(b1)
{
    matrix view;
    float4 viewPos;
}

cbuffer Light : register(b2)
{
    float4 lightDir;
    float4 lightColor;
};

cbuffer LightMatrix : register(b3)
{
    matrix lightViewProj;
}

cbuffer IrradianceVolumeParam : register(b4)
{
    IrradianceVolume volume;
};

float CalShadow(float3 P)
{
    float4 shadowPos = mul(float4(P, 1.0), lightViewProj);
    shadowPos.xy = shadowPos.xy * float2(0.5, -0.5) + 0.5;
    
    float shadow = 0.0;
    const float2 texelSize = 1.0 / float2(4096.0, 4096.0);
    
    [unroll]
    for (int x = -1; x < 2; x++)
    {
        [unroll]
        for (int y = -1; y < 2; y++)
        {
            shadow += shadowTexture.SampleCmpLevelZero(shadowSampler, shadowPos.xy + float2(x, y) * texelSize, shadowPos.z);
        }
    }
    
    shadow /= 9.0;
    
    return 1.0 - shadow;
}

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    const float3 position = gPosition.Sample(clampSampler, texCoord).xyz;
    const float4 normalSpecular = gNormalSpecular.Sample(clampSampler, texCoord);
    const float3 baseColor = gBaseColor.Sample(clampSampler, texCoord).rgb;
    
    float3 outColor = float3(0.0, 0.0, 0.0);
    
    const float3 N = normalize(normalSpecular.rgb);
    const float3 V = normalize(viewPos.xyz - position);
    
    const float NdotL = max(0.0, dot(N, lightDir.xyz));
    const float3 diffuseColor = lightColor.rgb * baseColor.rgb * NdotL;
        
    const float3 H = normalize(V + lightDir.xyz);
    const float NdotH = max(dot(N, H), 0.0);
    const float3 specularColor = lightColor.rgb * normalSpecular.w * pow(NdotH, 32.0);
   
    float shadow = CalShadow(position);
    
    outColor += (diffuseColor + specularColor) * CalShadow(position);
    
    outColor += baseColor * GetIndirectDiffuse(position, N, volume, irradianceCoeff, depthOctahedralMap, clampSampler);
    
    const float ao = ssaoTexture.Sample(clampSampler, texCoord).r;
        
    outColor *= ao;
    
    return float4(outColor, 1.0);
}