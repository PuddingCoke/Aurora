Texture2D gPosition : register(t0);
Texture2D gNormalSpecular : register(t1);
Texture2D gBaseColor : register(t2);
Texture2D ssaoTexture : register(t3);
Texture2D<float> shadowTexture : register(t4);
Texture3D<float4> voxelTexture : register(t5);

SamplerState wrapSampler : register(s0);
SamplerState linearSampler : register(s1);
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

cbuffer VoxelParam : register(b3)
{
    uint voxelGridRes;
    float voxelGridLength;
    float voxelSize;
    uint v4;
}

cbuffer LightMatrix : register(b4)
{
    matrix lightViewProj;
}

#define SQRT2 1.41421356237309504880
#define PI 3.14159265358979323846264

static const float maxMipLevel = 4.0;

//ConeTraceFactor
static const float ConeOffsetFactor = 2.0;
static const float ConeMaxLength = 300.0;
static const float ConeStepSize = 1.0;

//DifusseTraceFactor
static const uint numCones = 16;

//SpecularTraceFactor
static const float SpecularTraceRoughness = 0.3;

static const float ShadowTextureRes = 4096.0;

inline float3x3 getTangentSpace(float3 normal)
{
    float3 helper = abs(normal.x) > 0.99 ? float3(0, 0, 1) : float3(1, 0, 0);
    float3 tangent = normalize(cross(normal, helper));
    float3 binormal = normalize(cross(normal, tangent));
    return float3x3(tangent, binormal, normal);
}

inline float2 hammersley2d(uint idx, uint num)
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

inline float3 hemispherepoint_cos(float u, float v)
{
    float phi = v * 2 * PI;
    float cosTheta = sqrt(1 - u);
    float sinTheta = sqrt(1 - cosTheta * cosTheta);
    return float3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);
}

inline float4 ConeTrace(float3 P, float3 N, float3 coneDir, float coneAperture)
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
        
        float4 sam = voxelTexture.SampleLevel(linearSampler, tc, mipLevel);
        
        float a = 1.0 - alpha;
        color += a * sam.rgb;
        alpha += a * sam.a;
        
        dist += diameter * ConeStepSize;
    }
    
    
    return float4(color, alpha);
}

inline float4 TraceDiffuse(float3 P, float3 N)
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

inline float4 TraceSpecular(float3 P, float3 N, float3 V)
{
    float apeture = tan(SpecularTraceRoughness * PI * 0.5 * 0.1);
    float3 coneDir = reflect(-V, N);
    
    float4 amount = ConeTrace(P, N, coneDir, apeture);
    amount.rgb = max(0, amount.rgb);
    amount.a = saturate(amount.a);
    
    return amount;
}

float CalShadow(float3 P)
{
    float4 shadowPos = mul(float4(P, 1.0), lightViewProj);
    shadowPos.xyz / shadowPos.w;
    shadowPos.xy = shadowPos.xy * 0.5 + 0.5;
    shadowPos.y = 1.0 - shadowPos.y;
    
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
    const float3 position = gPosition.Sample(linearSampler, texCoord).xyz;
    const float4 normalSpecular = gNormalSpecular.Sample(linearSampler, texCoord);
    const float3 baseColor = gBaseColor.Sample(linearSampler, texCoord).rgb;
    
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
    
    outColor += baseColor.rgb * 0.05;
    
    float4 iDiffuse = TraceDiffuse(position, N);
    float4 iSpecular = TraceSpecular(position, N, V);
    
    outColor += baseColor.rgb * iDiffuse.rgb + iSpecular.rgb * normalSpecular.w;
    
    const float ao = ssaoTexture.Sample(linearSampler, texCoord).r;
        
    outColor *= ao;
    
    return float4(outColor, 1.0);
}