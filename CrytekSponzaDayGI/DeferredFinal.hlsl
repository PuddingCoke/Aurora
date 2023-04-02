Texture2D gPosition : register(t0);
Texture2D gNormalSpecular : register(t1);
Texture2D gBaseColor : register(t2);
Texture2D ssaoTexture : register(t3);
Texture2D<float> shadowTexture : register(t4);
Texture2DArray<float3> irradianceCoeff : register(t5);
Texture2DArray<float2> depthOctahedralMap : register(t6);

SamplerState wrapSampler : register(s0);
SamplerState clampSampler : register(s1);
SamplerComparisonState shadowSampler : register(s2);

cbuffer DeltaTimes : register(b0)
{
    float deltaTime;
    float sTime;
    float2 v0;
}

cbuffer ViewMatrix : register(b1)
{
    matrix view;
    float4 viewPos;
}

cbuffer Light : register(b2)
{
    float4 lightDir;
    float4 lightColor;
};

cbuffer LightMatrix : register(b3)
{
    matrix lightViewProj;
}

cbuffer IrradianceVolumeParam : register(b4)
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
    
    //[0,1] -> [1/18,17/18] avoid border sampling
    result = (result * 16.0 + 1.0) / 18.0;
    
    return result;
}

float2 GetDepth(const in float3 N, const in uint probeIndex)
{
    return depthOctahedralMap.Sample(clampSampler, float3(octEncode(N), float(probeIndex)));
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

float3 GetIrradiance(const in float3 N, const in uint probeIndex)
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

uint3 PosToProbeGridPos(float3 P)
{
    return uint3((P - fieldStart) / probeSpacing);
}

uint ProbeGridPosToIndex(uint3 probeGridPos)
{
    return probeGridPos.x + probeGridPos.z * probeCount.x + probeGridPos.y * probeCount.x * probeCount.z;
}

float3 ProbeGridPosToLoc(uint3 probeGridPos)
{
    return fieldStart + float3(probeGridPos) * probeSpacing;
}

float3 GetIndirectDiffuse(float3 P, float3 N)
{
    float3 sumIrradiance = float3(0.0, 0.0, 0.0);
    float sumWeight = 0.0;
    
    uint3 baseGridCoord = PosToProbeGridPos(P);
    
    float3 baseProbePos = ProbeGridPosToLoc(baseGridCoord);
    
    float3 alpha = clamp((P - baseProbePos) / probeSpacing, float3(0.0, 0.0, 0.0), float3(1.0, 1.0, 1.0));
    
    const uint3 offsets[8] =
    {
        { 0, 0, 0 },
        { 1, 0, 0 },
        { 1, 0, 1 },
        { 0, 0, 1 },
        { 0, 1, 0 },
        { 1, 1, 0 },
        { 1, 1, 1 },
        { 0, 1, 1 },
    };
    
    for (uint i = 0; i < 8; i++)
    {
        uint3 offset = offsets[i];
        
        uint3 probeGridCoord = clamp(baseGridCoord + offset, uint3(0, 0, 0), probeCount - uint3(1, 1, 1));
        
        uint probeIndex = ProbeGridPosToIndex(probeGridCoord);
        
        float3 probePos = ProbeGridPosToLoc(probeGridCoord);
        
        float3 probeToPoint = P - probePos;
        
        float3 dir = normalize(-probeToPoint);
        
        float distToProbe = length(probeToPoint);
        
        float3 trilinear = lerp(float3(1.0, 1.0, 1.0) - alpha, alpha, float3(offset));
        
        float weight = (trilinear.x * trilinear.y * trilinear.z) * max(0.005, dot(dir, N));
        
        float2 temp = GetDepth(-dir, probeIndex);
        
        float mean = temp.x;
        
        float variance = abs(temp.x * temp.x - temp.y);
        
        float chebyshevWeight = variance / (variance + (distToProbe - mean) * (distToProbe - mean));
        
        weight = max(0.00001, weight * ((distToProbe <= mean) ? 1.0 : chebyshevWeight));
        
        sumWeight += weight;
        
        sumIrradiance += weight * GetIrradiance(N, probeIndex);
    }
    
    return 0.5 * 3.1415926535 * sumIrradiance / sumWeight;
}

float CalShadow(float3 P)
{
    float4 shadowPos = mul(float4(P, 1.0), lightViewProj);
    shadowPos.xy = shadowPos.xy * float2(0.5, -0.5) + 0.5;
    
    float shadow = 0.0;
    const float2 texelSize = 1.0 / float2(4096.0, 4096.0);
    
    [unroll]
    for (int x = -1; x < 2; x++)
    {
        [unroll]
        for (int y = -1; y < 2; y++)
        {
            shadow += shadowTexture.SampleCmpLevelZero(shadowSampler, shadowPos.xy + float2(x, y) * texelSize, shadowPos.z);
        }
    }
    
    shadow /= 9.0;
    
    return 1.0 - shadow;
}

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    const float3 position = gPosition.Sample(clampSampler, texCoord).xyz;
    const float4 normalSpecular = gNormalSpecular.Sample(clampSampler, texCoord);
    const float3 baseColor = gBaseColor.Sample(clampSampler, texCoord).rgb;
    
    float3 outColor = float3(0.0, 0.0, 0.0);
    
    const float3 N = normalize(normalSpecular.rgb);
    const float3 V = normalize(viewPos.xyz - position);
    
    const float NdotL = max(0.0, dot(N, lightDir.xyz));
    const float3 diffuseColor = lightColor.rgb * baseColor.rgb * NdotL;
        
    const float3 H = normalize(V + lightDir.xyz);
    const float NdotH = max(dot(N, H), 0.0);
    const float3 specularColor = lightColor.rgb * normalSpecular.w * pow(NdotH, 32.0);
   
    float shadow = CalShadow(position);
    
    outColor += (diffuseColor + specularColor) * CalShadow(position);
    
    outColor += baseColor * GetIndirectDiffuse(position, N);
    
    const float ao = ssaoTexture.Sample(clampSampler, texCoord).r;
        
    outColor *= ao;
    
    return float4(outColor, 1.0);
}