Texture3D<float4> colorTexture : register(t0);
Texture3D<float4> normalTexture : register(t1);

RWTexture3D<float4> outputTexture : register(u0);

cbuffer VoxelParam : register(b1)
{
    uint voxelGridRes;
    float voxelGridLength;
    float voxelSize;
    float v0;
}

SamplerState linearClampSampler : register(s0);

#define SQRT2 1.41421356237309504880
#define PI 3.14159265358979323846264

static const float maxMipLevel = 4.0;

//ConeTraceFactor
static const float ConeOffsetFactor = 4.0;
static const float ConeMaxLength = 300.0;
static const float ConeStepSize = 1.0;

//DifusseTraceFactor
static const uint numCones = 8;

float3x3 getTangentSpace(float3 normal)
{
    float3 helper = abs(normal.x) > 0.99 ? float3(0, 0, 1) : float3(1, 0, 0);
    float3 tangent = normalize(cross(normal, helper));
    float3 binormal = normalize(cross(normal, tangent));
    return float3x3(tangent, binormal, normal);
}

float2 hammersley2d(uint idx, uint num)
{
    uint bits = idx;
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    const float radicalInverse_VdC = float(bits) * 2.3283064365386963e-10; // / 0x100000000

    return float2(float(idx) / float(num), radicalInverse_VdC);
}

float3 hemispherepoint_cos(float u, float v)
{
    float phi = v * 2 * PI;
    float cosTheta = sqrt(1 - u);
    float sinTheta = sqrt(1 - cosTheta * cosTheta);
    return float3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);
}

float4 ConeTrace(float3 P, float3 N, float3 coneDir, float coneAperture)
{
    float3 color = 0.0;
    float alpha = 0.0;
    
    float dist = voxelSize;
    float3 startPos = P + N * voxelSize * SQRT2 * ConeOffsetFactor;
    
    while (dist < ConeMaxLength && alpha < 1.0)
    {
        float diameter = max(voxelSize, 2.0 * coneAperture * dist);
        float mipLevel = log2(diameter / voxelSize);
        
        float3 tc = (startPos + coneDir * dist) / (voxelGridLength / 2.0);
        tc = tc * 0.5 + 0.5;
        
        if (tc.x != saturate(tc.x) || tc.y != saturate(tc.y) || tc.z != saturate(tc.z) || mipLevel >= maxMipLevel)
        {
            break;
        }
        
        float4 sam = colorTexture.SampleLevel(linearClampSampler, tc, mipLevel);
        
        float a = 1.0 - alpha;
        color += a * sam.rgb;
        alpha += a * sam.a;
        
        dist += diameter * ConeStepSize;
    }
    
    
    return float4(color, alpha);
}

float4 TraceDiffuse(float3 P, float3 N)
{
    float4 amount = 0.0;
    float3x3 tangentSpace = getTangentSpace(N);
    
    for (uint cone = 0; cone < numCones; cone++)
    {
        float2 hamm = hammersley2d(cone, numCones);
        float3 hemisphere = hemispherepoint_cos(hamm.x, hamm.y);
        float3 coneDir = mul(hemisphere, tangentSpace);
        
        amount += ConeTrace(P, N, coneDir, tan(PI * 0.5 * 0.33));
    }
    
    amount /= float(numCones);
    amount.rgb = max(0, amount.rgb);
    amount.a = saturate(amount.a);
    
    return amount;
}

[numthreads(8, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    float4 emission = colorTexture[DTid];
    
    if(emission.a>0.1)
    {
        float3 N = normalTexture[DTid].xyz;
        
        float3 P = ((float3(DTid) + 0.5) / float(voxelGridRes) - 0.5) * voxelGridLength;
        
        float4 radiance = TraceDiffuse(P, N);
        
        outputTexture[DTid] = emission + float4(radiance.rgb, 0.0);
    }
    else
    {
        outputTexture[DTid] = float4(0.0, 0.0, 0.0, 0.0);
    }
    
}