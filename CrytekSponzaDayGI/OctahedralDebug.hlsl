Texture2DArray<float2> depthOctahedralMap : register(t0);

SamplerState linearClampSampler : register(s0);

float signNotZero(float k)
{
    return k >= 0.0 ? 1.0 : -1.0;
}

float2 signNotZero(float2 v)
{
    return float2(signNotZero(v.x), signNotZero(v.y));
}

float2 octEncode(float3 v)
{
    float l1norm = abs(v.x) + abs(v.y) + abs(v.z);
    
    float2 result = v.xy * (1.0 / l1norm);
    if (v.z < 0.0)
    {
        result = (1.0 - abs(result.yx)) * signNotZero(result.xy);
    }
    
    //[-1,1] -> [0,1]
    result = (result + 1.0) / 2.0;
    
    //[0,1] -> [1/18,17/18] avoid border sampling
    result = (result * 16.0 + 1.0) / 18.0;
    
    return result;
}

float2 GetDepth(const in float3 N, const in uint probeIndex)
{
    return depthOctahedralMap.Sample(linearClampSampler, float3(octEncode(N), float(probeIndex)));
}

float4 main(float3 position : POSITION) : SV_TARGET
{
    return float4(GetDepth(normalize(position), 0).rrr / 512.0, 1.0f);
}