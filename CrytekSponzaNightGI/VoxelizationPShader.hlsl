struct PixelInput
{
    float3 pos : POSITION0;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT0;
    float3 bitangent : TANGENT1;
};

struct Light
{
    float4 position;
    float4 color;
    float radius;
    float quadraticFalloff;
    float linearFalloff;
    float v0;
};

cbuffer LightInfo : register(b1)
{
    Light lights[10];
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

RWTexture3D<uint> voxelTextureColor : register(u0);
RWTexture3D<uint> voxelTextureNormal : register(u1);

Texture2D tDiffuse : register(t0);
Texture2D tSpecular : register(t1);
Texture2D tNormal : register(t2);

SamplerState samplerState : register(s0);

uint PackFloat4(float4 val)
{
    return (uint(val.a) & 0x000000FF) << 24U | (uint(val.b) & 0x000000FF) << 16U | (uint(val.g) & 0x000000FF) << 8U | (uint(val.r) & 0x000000FF);
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
    
    float4 color = float4(0.0, 0.0, 0.0, 1.0);
    
    [unroll]
    for (uint i = 0; i < 10; i++)
    {
        float3 L = lights[i].position.xyz - input.pos.xyz;
        
        float atten = lights[i].radius / (pow(length(L), 2.0) + 1.0);
        
        L = normalize(L);
        
        float NDotL = max(0.0, dot(N, L));
        
        color.rgb += lights[i].color.rgb * baseColor.rgb * NDotL * atten;
    }
    
    float3 pos = input.pos / (voxelGridLength / 2.0);
    pos = (pos * 0.5 + 0.5) * float(voxelGridRes);
    
    color *= 255.0;
    N = (N + 1.0) / 2.0 * 255.0;
    
    InterlockedMax(voxelTextureColor[uint3(pos)], PackFloat4(color));
    InterlockedMax(voxelTextureNormal[uint3(pos)], PackFloat4(float4(N, 255.0)));
}