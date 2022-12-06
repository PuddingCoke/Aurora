Texture2D originTexture : register(t0);
Texture2D gPosition : register(t1);
Texture2D gNormal : register(t2);
Texture2D<float> depthView : register(t3);

SamplerState pointSampler : register(s0);

cbuffer ViewInfo : register(b1)
{
    matrix view;
    float4 viewPos;
    matrix prevViewProj;
    matrix viewProj;
}

static const float depthBias = 0.0002;

float3 GetUVCoord(float3 pos)
{
    float4 projPos = mul(float4(pos, 1.0), viewProj);
    projPos /= projPos.w;
    projPos.xy = projPos.xy * 0.5 + 0.5;
    projPos.y = 1.0 - projPos.y;
    return projPos.xyz;
}

float3 BinarySearch(float3 dir, float3 pos)
{
    [unroll]
    for (uint i = 0; i < 40; i++)
    {
        float3 uv = GetUVCoord(pos);
        float srcDepth = depthView.SampleLevel(pointSampler, uv.xy, 0.0).x;
        float depthDiff = srcDepth - uv.z;
        
        if (depthDiff > 0.0)
        {
            pos += dir;
            dir *= 0.5;
        }
        
        pos -= dir;
    }
    
    float3 uv = GetUVCoord(pos);
    float srcDepth = depthView.SampleLevel(pointSampler, uv.xy, 0.0).x;
    float depthDiff = abs(srcDepth - uv.z);
    
    float3 result = float3(0.0, 0.0, 0.0);
    
    if (uv.z < 1.0 && depthDiff < depthBias)
    {
        result = originTexture.SampleLevel(pointSampler, uv.xy, 0.0).rgb;
    }
    
    return result;
}

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    const float3 P = gPosition.Sample(pointSampler, texCoord).xyz;
    const float3 N = normalize(gNormal.Sample(pointSampler, texCoord).xyz);
    
    const float3 dir = normalize(P - viewPos.xyz);
    
    float3 reflectDir = reflect(dir, N) * 0.1;
    
    float3 reflectPos = P;
    
    float3 color = originTexture.Sample(pointSampler, texCoord, 0.0).rgb;
    
    [loop]
    for (uint i = 0; i < 100; i++)
    {
        float3 uv = GetUVCoord(reflectPos);
        float srcDepth = depthView.SampleLevel(pointSampler, uv.xy, 0.0).x;
        
        float depthDiff = uv.z - srcDepth;
        
        if (depthDiff > depthBias && depthDiff < 0.0005)
        {
            color += BinarySearch(reflectDir, reflectPos);
            break;
        }
        else
        {
            reflectPos += reflectDir;
        }
    }
    
    return float4(color, 1.0);
}