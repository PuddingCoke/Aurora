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

#define SAMPLECOUNT 64

static const float3 randomDirection[SAMPLECOUNT] =
{
    float3(0.556353, 0.299828, 0.774968),
    float3(0.297586, 0.475474, 0.827869),
    float3(-0.0859018, 0.225413, 0.970469),
    float3(-0.467865, 0.0466926, 0.882565),
    float3(-0.653394, -0.0324445, 0.756323),
    float3(-0.117416, -0.152125, 0.981362),
    float3(0.290694, -0.426662, 0.856421),
    float3(0.388092, -0.227603, 0.893074),
    float3(0.716246, 0.00257923, 0.697843),
    float3(0.504986, 0.542651, 0.671207),
    float3(-0.0396573, 0.839854, 0.541362),
    float3(-0.784278, 0.33775, 0.520415),
    float3(-0.729103, -0.128723, 0.67219),
    float3(-0.129665, -0.834079, 0.536189),
    float3(0.370427, -0.606067, 0.703894),
    float3(0.76946, -0.0168703, 0.638472),
    float3(0.724339, 0.571555, 0.385562),
    float3(0.437745, 0.788662, 0.431731),
    float3(-0.373934, 0.868842, 0.324479),
    float3(-0.813559, 0.347351, 0.466336),
    float3(-0.91118, -0.0477759, 0.409228),
    float3(-0.415242, -0.865478, 0.280218),
    float3(0.0868553, -0.947342, 0.308219),
    float3(0.78846, -0.550646, 0.27408),
    float3(0.931568, 0.362363, 0.0295752),
    float3(0.203868, 0.951739, 0.229412),
    float3(-0.065404, 0.985923, 0.15388),
    float3(-0.710939, 0.693258, 0.118146),
    float3(-0.716907, -0.657643, 0.231408),
    float3(-0.197943, -0.979167, 0.0452791),
    float3(0.548249, -0.833655, 0.0666598),
    float3(0.838604, -0.5144, 0.179265),
    float3(0.84193, 0.485021, -0.23645),
    float3(0.0615243, 0.981923, -0.179005),
    float3(-0.0805242, 0.976073, -0.201984),
    float3(-0.871225, 0.459621, -0.172382),
    float3(-0.888195, -0.442754, -0.122796),
    float3(-0.132939, -0.983914, -0.119336),
    float3(0.511186, -0.842927, -0.16782),
    float3(0.914608, -0.388928, -0.110575),
    float3(0.924571, 0.145544, -0.352115),
    float3(0.366873, 0.887769, -0.277975),
    float3(-0.0541902, 0.869993, -0.490077),
    float3(-0.891824, 0.344039, -0.293745),
    float3(-0.888081, -0.276162, -0.367487),
    float3(-0.447291, -0.837005, -0.315202),
    float3(0.473813, -0.785507, -0.398096),
    float3(0.838151, -0.450172, -0.307973),
    float3(0.68559, 0.284173, -0.670233),
    float3(0.0473995, 0.861911, -0.504839),
    float3(-0.0579678, 0.682809, -0.728293),
    float3(-0.60115, 0.337818, -0.724222),
    float3(-0.684874, -0.0950813, -0.722431),
    float3(-0.100279, -0.714356, -0.69256),
    float3(0.461067, -0.684422, -0.564786),
    float3(0.788861, -0.255134, -0.55911),
    float3(0.425033, 0.0776907, -0.901838),
    float3(0.200657, 0.561227, -0.80297),
    float3(-0.028863, 0.527474, -0.849081),
    float3(-0.333382, 0.0336013, -0.942193),
    float3(-0.359787, -0.189299, -0.91363),
    float3(-0.0347407, -0.310978, -0.949782),
    float3(0.211939, -0.435887, -0.874691),
    float3(0.600673, -0.276017, -0.750337)
};

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
        
        [unroll]
        for (uint i = 0; i < SAMPLECOUNT; i++)
        {
            const float3 randDir = normalize(dir + randomDirection[i]);

            const float depth = depthCube.SampleLevel(linearClampSampler, randDir, 0.0);
            
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