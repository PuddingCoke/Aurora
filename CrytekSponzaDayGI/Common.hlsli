struct IrradianceVolume
{
    float3 fieldStart;
    float probeSpacing;
    uint3 probeCount;
    float padding;
};

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
    
    //[0,1] -> [1/16,15/16] avoid border sampling
    result = (result * 14.0 + 1.0) / 16.0;
    
    return result;
}

float2 GetDepth(in float3 N, in uint probeIndex, in Texture2DArray<float2> depthOctahedralMap, in SamplerState samplerState)
{
    return depthOctahedralMap.Sample(samplerState, float3(octEncode(N), float(probeIndex)));
}

#define A0 3.141592653589793
#define A1 2.094395102393195
#define A2 0.785398163397448

#define C0 0.2820947917738781
#define C1 0.4886025119029199
#define C2 0.4886025119029199
#define C3 0.4886025119029199
#define C4 1.0925484305920790
#define C5 1.0925484305920790
#define C6 0.3153915652525200
#define C7 1.0925484305920790
#define C8 0.5462742152960395

float3 GetIrradiance(in float3 N, in uint probeIndex, in Texture2DArray<float3> irradianceCoeff)
{
    return max(float3(0.0, 0.0, 0.0),
    A0 * C0 * irradianceCoeff[uint3(0, 0, probeIndex)]
    - A1 * C1 * irradianceCoeff[uint3(1, 0, probeIndex)] * N.y
    + A1 * C2 * irradianceCoeff[uint3(2, 0, probeIndex)] * N.z
    - A1 * C3 * irradianceCoeff[uint3(3, 0, probeIndex)] * N.x
    + A2 * C4 * irradianceCoeff[uint3(4, 0, probeIndex)] * (N.y * N.x)
    - A2 * C5 * irradianceCoeff[uint3(5, 0, probeIndex)] * (N.y * N.z)
    + A2 * C6 * irradianceCoeff[uint3(6, 0, probeIndex)] * (3.0 * N.z * N.z - 1.0)
    - A2 * C7 * irradianceCoeff[uint3(7, 0, probeIndex)] * (N.x * N.z)
    + A2 * C8 * irradianceCoeff[uint3(8, 0, probeIndex)] * (N.x * N.x - N.y * N.y));
}

uint3 PosToProbeGridPos(in float3 P, in IrradianceVolume volume)
{
    return uint3((P - volume.fieldStart) / volume.probeSpacing);
}

uint ProbeGridPosToIndex(in uint3 probeGridPos, in IrradianceVolume volume)
{
    return probeGridPos.x + probeGridPos.z * volume.probeCount.x + probeGridPos.y * volume.probeCount.x * volume.probeCount.z;
}

float3 ProbeGridPosToLoc(in uint3 probeGridPos, in IrradianceVolume volume)
{
    return volume.fieldStart + float3(probeGridPos) * volume.probeSpacing;
}

float3 GetIndirectDiffuse(in float3 P, in float3 N, in IrradianceVolume volume, in Texture2DArray<float3> irradianceCoeff, in Texture2DArray<float2> depthOctahedralMap, in SamplerState samplerState)
{
    float3 sumIrradiance = float3(0.0, 0.0, 0.0);
    
    float sumWeight = 0.0;
    
    uint3 baseGridCoord = PosToProbeGridPos(P, volume);
    
    float3 baseProbePos = ProbeGridPosToLoc(baseGridCoord, volume);
    
    float3 alpha = clamp((P - baseProbePos) / volume.probeSpacing, float3(0.0, 0.0, 0.0), float3(1.0, 1.0, 1.0));
    
    for (uint i = 0; i < 8; i++)
    {
        uint3 offset = uint3(i, i >> 1, i >> 2) & uint3(1, 1, 1);
        
        uint3 probeGridCoord = clamp(baseGridCoord + offset, uint3(0, 0, 0), uint3(volume.probeCount - 1));
        
        uint probeIndex = ProbeGridPosToIndex(probeGridCoord, volume);
        
        float3 probePos = ProbeGridPosToLoc(probeGridCoord, volume);
        
        float3 probeToPoint = P - probePos;
        
        float3 dir = normalize(-probeToPoint);
        
        float distToProbe = length(probeToPoint);
        
        float3 trilinear = lerp(float3(1.0, 1.0, 1.0) - alpha, alpha, float3(offset));
        
        float weight = (trilinear.x * trilinear.y * trilinear.z) * max(0.005, dot(dir, N));
        
        float2 temp = GetDepth(-dir, probeIndex, depthOctahedralMap, samplerState);
        
        float mean = temp.x;
        
        float variance = abs(temp.x * temp.x - temp.y);
        
        float chebyshevWeight = variance / (variance + (distToProbe - mean) * (distToProbe - mean));
        
        weight = max(0.00001, weight * ((distToProbe <= mean) ? 1.0 : chebyshevWeight));
        
        sumWeight += weight;
        
        sumIrradiance += weight * GetIrradiance(N, probeIndex, irradianceCoeff);
    }
    
    return 0.5 * 3.1415926535 * 1.0989 * sumIrradiance / sumWeight;
}