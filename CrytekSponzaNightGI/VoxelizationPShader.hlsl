struct PixelInput
{
    float3 pos : POSITION0;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT0;
    float3 bitangent : TANGENT1;
    float4 svPosition : SV_Position;
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
    Light lights[8];
};

cbuffer ViewMatrix : register(b2)
{
    matrix view;
    matrix normalMatrix;
    float4 viewPos;
}

cbuffer VoxelParam : register(b3)
{
    uint voxelGridLength;
    uint voxelGridRes;
    uint2 v3;
}

RWTexture3D<uint> voxelTexture : register(u0);

Texture2D tDiffuse : register(t0);
Texture2D tSpecular : register(t1);
Texture2D tNormal : register(t2);

SamplerState samplerState : register(s0);

uint Float4ToRGBA8(float4 val)
{
    return (uint(val.w) & 0x000000FF) << 24U |
	 (uint(val.z) & 0x000000FF) << 16U |
	 (uint(val.y) & 0x000000FF) << 8U |
	 (uint(val.x) & 0x000000FF);
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
    
    float3 normal = tNormal.Sample(samplerState, input.uv).xyz * 2.0 - 1.0;
    normal = mul(normalize(normal), TBN);
    
    float4 color = float4(0.0, 0.0, 0.0, 1.0);
    
    [unroll]
    for (uint i = 0; i < 8; i++)
    {
        float3 L = lights[i].position.xyz - input.pos.xyz;
        
        float atten = lights[i].radius / (pow(length(L), 2.0) + 1.0);
        
        L = normalize(L);
        
        float NDotL = max(0.0, dot(normal, L));
        
        color.rgb += lights[i].color.rgb * baseColor.rgb * NDotL * atten;
    }
    
    color *= 255.0;
    
    float3 pos = input.pos / (float(voxelGridLength) / 2.0);
    pos = (pos * 0.5 + 0.5) * float(voxelGridRes - 1);
    
    InterlockedMax(voxelTexture[uint3(pos)], Float4ToRGBA8(color));
}