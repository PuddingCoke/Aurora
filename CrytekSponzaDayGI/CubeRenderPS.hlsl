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
    float4 probeLocation;
};

static const float ShadowTextureRes = 4096.0;

float CalShadow(float3 P)
{
    float4 shadowPos = mul(float4(P, 1.0), lightViewProj);
    shadowPos.xy = shadowPos.xy * float2(0.5, -0.5) + 0.5;
    
    float shadow = 0.0;
    const float2 texelSize = 1.0 / float2(ShadowTextureRes, ShadowTextureRes);
    
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

float4 main(PixelInput input) : SV_TARGET
{
    const float4 baseColor = tDiffuse.Sample(wrapSampler, input.uv);
    
    if (baseColor.a < 0.5)
    {
        discard;
    }
    
    const float specular = tSpecular.Sample(wrapSampler, input.uv).r;
    
    const float3 V = normalize(probeLocation.xyz - input.pos);
    
    const float NdotL = max(0.0, dot(input.normal, lightDir.xyz));
    const float3 diffuseColor = lightColor.rgb * baseColor.rgb * NdotL;
        
    const float3 H = normalize(V + lightDir.xyz);
    const float NdotH = max(dot(input.normal, H), 0.0);
    const float3 specularColor = lightColor.rgb * specular * pow(NdotH, 32.0);
   
    float shadow = CalShadow(input.pos);
    
    return float4((diffuseColor + specularColor) * CalShadow(input.pos), 1.0);
}