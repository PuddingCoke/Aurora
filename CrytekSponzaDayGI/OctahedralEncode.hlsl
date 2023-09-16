#include"Common.hlsli"

RWTexture2DArray<float2> depthOctahedralMap : register(u0);

TextureCube<float> depthCube : register(t0);

SamplerState linearClampSampler : register(s0);

cbuffer ProjMatrices : register(b1)
{
    matrix viewProj[6];
    float3 probeLocation;
    uint probeIndex;
};

#define TWO_PI     6.2831853071
#define SAMPLECOUNT 64


static float hashSeed = 0.0;

uint BaseHash(uint2 p)
{
    p = 1103515245U * ((p >> 1U) ^ (p.yx));
    uint h32 = 1103515245U * ((p.x) ^ (p.y >> 3U));
    return h32 ^ (h32 >> 16);
}

float3 Hash3(inout float seed)
{
    uint n = BaseHash(asuint(float2(seed += 0.1, seed += 0.1)));
    uint3 rz = uint3(n, n * 16807U, n * 48271U);
    return float3(rz & uint3(0x7fffffffU, 0x7fffffffU, 0x7fffffffU)) / float(0x7fffffff);
}

float3 RandomPointInUnitSphere(inout float seed)
{
    float3 h = Hash3(seed) * float3(TWO_PI, 2.0, 1.0) - float3(0.0, 1.0, 0.0);
    float theta = h.x;
    float sinPhi = sqrt(1.0 - h.y * h.y);
    float r = pow(h.z, 0.3333333334);
    
    return r * float3(cos(theta) * sinPhi, sin(theta) * sinPhi, h.y);
}

[numthreads(16, 16, 1)]
void main(uint2 DTid : SV_DispatchThreadID)
{
    bool isBorderTexel = (DTid.x == 0 || DTid.x == 15) | (DTid.y == 0 || DTid.y == 15);
    
    //update interior texel
    if (!isBorderTexel)
    {
        const float2 coor = (float2(DTid - uint2(1, 1)) + 0.5) / 7.0 - float2(1.0, 1.0);
    
        const float3 dir = octDecode(coor.x, coor.y);
    
        float radialDepth = depthCube.SampleLevel(linearClampSampler, dir, 0.0);
        
        float radialDepthSquare = radialDepth * radialDepth;
        
        for (uint i = 0; i < SAMPLECOUNT; i++)
        {
            float3 randDir = normalize(dir + RandomPointInUnitSphere(hashSeed));

            float depth = depthCube.SampleLevel(linearClampSampler, randDir, 0.0);
            
            radialDepth += depth;
            
            radialDepthSquare += depth * depth;
        }
        
        radialDepth /= float(SAMPLECOUNT) + 1.0;
        
        radialDepthSquare /= float(SAMPLECOUNT) + 1.0;
        
        depthOctahedralMap[uint3(DTid, probeIndex)] = float2(radialDepth, radialDepthSquare);
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