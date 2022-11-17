Texture2D originTexture : register(t0);
Texture2D gViewSpacePosition : register(t1);
Texture2D gNormal : register(t2);

SamplerState borderSampler : register(s0);

cbuffer ProjMatrix : register(b1)
{
    matrix proj;
}

cbuffer ViewInfo : register(b2)
{
    matrix view;
    float4 viewPos;
    matrix prevViewProj;
    matrix viewProj;
    matrix normalMatrix;
}

static const float depthBias = 0.0001;

inline float2 GetUVCoord(float3 pos)
{
    float4 projPos = mul(float4(pos, 1.0), proj);
    projPos.xy /= projPos.w;
    projPos.xy = projPos.xy * float2(0.5, -0.5) + float2(0.5, 0.5);
    return projPos.xy;
}

float4 BinarySearch(float3 dir, float3 pos)
{
    [unroll]
    for (uint i = 0; i < 20; i++)
    {
        dir *= 0.5;
        
        float2 uv = GetUVCoord(pos);
        float depth = gViewSpacePosition.SampleLevel(borderSampler, uv, 0.0).z;
        float depthDiff = depth - pos.z;
        
        if (depthDiff > 0.0)
        {
            pos += dir;
        }
        else
        {
            pos -= dir;
        }
    }
    
    float2 uv = GetUVCoord(pos);
    float depth = gViewSpacePosition.SampleLevel(borderSampler, uv, 0.0).z;
    float depthDiff = abs(depth - pos.z);
    
    float4 result = float4(0.0, 0.0, 0.0, 1.0);

    result = originTexture.SampleLevel(borderSampler, uv, 0.0);
    
    return result;
}

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    const float3 P = gViewSpacePosition.Sample(borderSampler, texCoord).xyz;
    const float3 N = normalize(mul(normalize(gNormal.Sample(borderSampler, texCoord).xyz), (float3x3) normalMatrix));
    
    const float3 dir = normalize(P);
    float3 reflectDir = reflect(dir, N);
    
    float3 reflectPos = P;
    
    [loop]
    for (uint i = 0; i < 300; i++)
    {
        reflectPos += reflectDir;
        
        float2 uv = GetUVCoord(reflectPos);
        
        if (saturate(uv.x) != uv.x || saturate(uv.y) != uv.y)
        {
            break;
        }
        
        float depth = gViewSpacePosition.SampleLevel(borderSampler, uv.xy, 0.0).z;
        
        float depthDiff = reflectPos.z - depth;
        
        if (depthDiff > 0.1 && depthDiff < 2.0)
        {
            return BinarySearch(reflectDir, reflectPos);
        }
        
    }
    
    return float4(0.0, 0.0, 0.0, 1.0);
}