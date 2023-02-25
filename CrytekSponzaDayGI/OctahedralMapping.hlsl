RWTexture2DArray<float2> depthOctahedralMap : register(u0);

TextureCube<float> depthCube : register(t0);
SamplerState linearClampSampler : register(s0);

cbuffer ProjMatrices : register(b1)
{
    matrix viewProj[6];
    float3 probeLocation;
    uint probeIndex;
};

float signNotZero(float k)
{
    return k >= 0.0 ? 1.0 : -1.0;
}

float2 signNotZero(float2 v)
{
    return float2(signNotZero(v.x), signNotZero(v.y));
}

float3 octDecode(float x, float y)
{
    float3 v = float3(x, y, 1.0 - abs(x) - abs(y));
    if (v.z < 0)
    {
        v.xy = (1.0 - abs(v.yx)) * signNotZero(v.xy);
    }
    return normalize(v);
}

[numthreads(16, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    float2 coor = (float2(DTid.xy) + 0.5) / 8.0 - float2(1.0, 1.0);
    float3 dir = octDecode(coor.x, coor.y);
    float depth = depthCube.SampleLevel(linearClampSampler, dir, 0.0);
    depthOctahedralMap[uint3(DTid.xy, probeIndex)] = float2(depth, depth * depth);
}