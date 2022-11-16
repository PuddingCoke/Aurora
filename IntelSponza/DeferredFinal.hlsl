Texture2D gBaseColor : register(t0);
Texture2D gPosition : register(t1);
Texture2D gNormal : register(t2);
Texture2D gRoughnessMetallic : register(t3);
Texture2D<float> hbaoTexture : register(t4);

SamplerState pointSampler : register(s0);

#define PI 3.14159265358979323846264

cbuffer ViewInfo : register(b1)
{
    matrix view;
    float4 viewPos;
    matrix prevViewProj;
    matrix viewProj;
    matrix normalMatrix;
};

struct Light
{
    float4 position;
    float4 color;
    float radius;
    float quadraticFalloff;
    float linearFalloff;
    float v0;
};

cbuffer LightInfo : register(b2)
{
    Light lights[22];
}

float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

float3 fresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    float3 baseColor = gBaseColor.Sample(pointSampler, texCoord).rgb;
    float2 roughnessMetallic = gRoughnessMetallic.Sample(pointSampler, texCoord).rg;
    
    float3 P = gPosition.Sample(pointSampler, texCoord).xyz;
    float3 N = normalize(gNormal.Sample(pointSampler, texCoord).rgb);
    float3 V = normalize(viewPos.xyz - P);
    
    float roughness = roughnessMetallic.r;
    float metallic = roughnessMetallic.g;
    
    float3 F0 = float3(0.04, 0.04, 0.04);
    F0 = lerp(F0, baseColor, metallic);
    
    float3 L0 = float3(0.0, 0.0, 0.0);
    
    [unroll]
    for (uint i = 0; i < 22; i++)
    {
        float3 L = normalize(lights[i].position.xyz - P);
        float3 H = normalize(V + L);
        float distance = length(lights[i].position.xyz - P);
        float atten = lights[i].radius / (distance * distance + 1.0);
        float3 radiance = lights[i].color.rgb * atten;
        
        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        float3 F = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);
    
        float3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        float3 specular = numerator / denominator;
    
        float3 kS = F;
    
        float3 kD = float3(1.0, 1.0, 1.0) - kS;
    
        kD *= 1.0 - metallic;
    
        float NdotL = max(dot(N, L), 0.0);
    
        L0 += (kD * baseColor / PI + specular) * radiance * NdotL;
    }
    
    float ao = hbaoTexture.Sample(pointSampler, texCoord).r;
    
    L0 *= ao;
    
    return float4(L0, 1.0);
}