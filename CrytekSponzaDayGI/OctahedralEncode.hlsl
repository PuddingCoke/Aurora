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
void main(uint2 DTid : SV_DispatchThreadID)
{
    bool isBorderTexel = (DTid.x == 0 || DTid.x == 15) | (DTid.y == 0 || DTid.y == 15);
    
    //update interior texel
    if (!isBorderTexel)
    {
        float2 coor = (float2(DTid - uint2(1, 1)) + 0.5) / 7.0 - float2(1.0, 1.0);
    
        float3 dir = octDecode(coor.x, coor.y);
    
        float radialDepth = depthCube.SampleLevel(linearClampSampler, dir, 0.0);
    
        depthOctahedralMap[uint3(DTid, probeIndex)] = float2(radialDepth, radialDepth * radialDepth);
    }
    
    AllMemoryBarrierWithGroupSync();
    
    //update border texel
    if (isBorderTexel)
    {
        bool isCornerTexel = (DTid.x == 0 || DTid.x == 15) && (DTid.y == 0 || DTid.y == 15);
        bool isRowTexel = (DTid.x > 0 && DTid.x < 15);

        uint2 copyCoor;

        if (isCornerTexel)
        {
            copyCoor.x = DTid.x > 0 ? 1 : 14;
            copyCoor.y = DTid.y > 0 ? 1 : 14;
        }
        else if (isRowTexel)
        {
            copyCoor.x = 15 - DTid.x;
            copyCoor.y = DTid.y + ((DTid.y > 0) ? -1 : 1);
        }
        else
        {
            copyCoor.x = DTid.x + ((DTid.x > 0) ? -1 : 1);
            copyCoor.y = 15 - DTid.y;
        }

        depthOctahedralMap[uint3(DTid, probeIndex)] = depthOctahedralMap[uint3(copyCoor, probeIndex)];
    }
}