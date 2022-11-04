struct PixelInput
{
    float3 pos : POSITION0;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT0;
    float3 bitangent : TANGENT1;
};

cbuffer Light : register(b1)
{
    float4 lightDir;
    float4 lightColor;
};

cbuffer ViewMatrix : register(b2)
{
    matrix view;    
    float4 viewPos;
}

cbuffer VoxelParam : register(b3)
{
    uint voxelGridRes;
    float voxelGridLength;
    uint2 v3;
}

cbuffer LightMatrix : register(b4)
{
    matrix lightViewProj;
}

RWTexture3D<uint> voxelTextureTempColor : register(u0);
RWTexture3D<uint> voxelTextureTempNormal : register(u1);

Texture2D tDiffuse : register(t0);
Texture2D tSpecular : register(t1);
Texture2D tNormal : register(t2);
Texture2D<float> shadowTexture : register(t3);

SamplerState samplerState : register(s0);
SamplerComparisonState shadowSampler : register(s1);

uint Float4ToRGBA8(in float4 val)
{
    return (uint(val.w) & 0x000000FF) << 24U | (uint(val.z) & 0x000000FF) << 16U | (uint(val.y) & 0x000000FF) << 8U | (uint(val.x) & 0x000000FF);
}

float CalShadow(float3 P)
{
    float4 shadowPos = mul(float4(P, 1.0), lightViewProj);
    shadowPos.xyz / shadowPos.w;
    shadowPos.xy = shadowPos.xy * 0.5 + 0.5;
    shadowPos.y = 1.0 - shadowPos.y;
    
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
    
    return shadow;
}

void main(PixelInput input)
{
    float4 baseColor = tDiffuse.Sample(samplerState, input.uv);
 
    [branch]
    if (baseColor.a < 0.5)
    {
        discard;
    }
 
    float3 N = normalize(input.normal);
    float3 B = normalize(input.bitangent);
    float3 T = normalize(input.tangent);
    float3x3 TBN = float3x3(T, B, N);
    
    const float3 normal = tNormal.Sample(samplerState, input.uv).xyz * 2.0 - 1.0;
    N = mul(normalize(normal), TBN);
    
    float NDotL = max(0.0, dot(N, lightDir.xyz));
    
    float shadow = CalShadow(input.pos.xyz);
    
    float4 color = float4(lightColor.rgb * baseColor.rgb * NDotL * (1.0 - shadow), 1.0);
    
    float3 pos = input.pos / (voxelGridLength / 2.0);
    pos = (pos * 0.5 + 0.5) * float(voxelGridRes);
    
    color *= 255.0;
    N = (N + 1.0) / 2.0 * 255.0;
    
    InterlockedMax(voxelTextureTempColor[uint3(pos)], Float4ToRGBA8(color));
    InterlockedMax(voxelTextureTempNormal[uint3(pos)], Float4ToRGBA8(float4(N, 255.0)));
}