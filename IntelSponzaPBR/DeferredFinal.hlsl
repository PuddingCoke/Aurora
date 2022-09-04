Texture2D tPosition : register(t0);
Texture2D tBaseColor : register(t1);
Texture2D tMetallicRoughness : register(t2);
Texture2D tNormal : register(t3);
Texture2D tLightSpacePos : register(t4);
Texture2D tShadow : register(t5);

SamplerState wrapSampler : register(s0);
SamplerState linearSampler : register(s1);
SamplerState borderSampler : register(s2);

cbuffer LightBuffer : register(b3)
{
    float4 camPos;
    float4 sunDir;
}

static const float PI = 3.14159265359;

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

float3 fresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
{
    return F0 + (max(float3(1.0 - roughness, 1.0 - roughness, 1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float ShadowCalculation(float2 texCoord)
{
    float3 coord = tLightSpacePos.Sample(linearSampler, texCoord).xyz;
    coord = float3(coord.xy * 0.5 + 0.5, coord.z);
    coord.y = 1.0 - coord.y;
    
    float bias = 0.001;
    
    float shadow = 0.0;
    float2 texelSize;
    tShadow.GetDimensions(texelSize.x, texelSize.y);
    texelSize = 1.0 / texelSize;
    for (int x = -2; x <= 2; ++x)
    {
        for (int y = -2; y <= 2; ++y)
        {
            float pcfDepth = tShadow.Sample(borderSampler, coord.xy + float2(x, y) * texelSize).r;
            shadow += coord.z - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 25.0;
    
    if (coord.z > 1.0)
        shadow = 0.0;
    
    return shadow;
}

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    float3 baseColor = tBaseColor.Sample(linearSampler, texCoord).rgb;
    
    float2 metallicRoughness = tMetallicRoughness.Sample(linearSampler, texCoord).rg;
    
    float metallic = metallicRoughness.r;
    
    float roughness = metallicRoughness.g;
    
    float3 positon = tPosition.Sample(linearSampler, texCoord).rgb;
    
    float3 N = tNormal.Sample(linearSampler, texCoord).rgb;
    
    float3 V = normalize(camPos.xyz - positon);
    
    float3 F0 = float3(0.04, 0.04, 0.04);
    F0 = lerp(F0, baseColor, metallic);
    
    float3 Lo = float3(0, 0, 0);
    {
    
        float3 L = -sunDir.xyz;
    
        float3 H = normalize(V + L);
    
        float3 radiance = float3(1.0, 1.0, 1.0) * 12.0;
    
        float NDF = DistributionGGX(N, H, roughness);
    
        float G = GeometrySmith(N, V, L, roughness);
    
        float3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
    
        float3 numerator = NDF * G * F;
    
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    
        float3 specular = numerator / denominator;
    
        float3 kS = F;
    
        float3 kD = float3(1.0, 1.0, 1.0) - kS;
    
        kD *= 1.0 - metallic;
    
        float NdotL = max(dot(N, L), 0.0);
    
        Lo = (kD * baseColor / PI + specular) * radiance * NdotL;
    }
    
    float3 L2 = float3(0.0,0.0,0.0);
    {
    
        float3 L = normalize(float3(0, 5.0, 0) - positon);
    
        float3 H = normalize(V + L);
    
        float distance = length(float3(0, 10.0, 0) - positon);
        
        float attenuation = 1.0 / (distance * distance);
        
        float3 radiance = float3(15.0, 15.0, 15.0) * attenuation;
    
        float NDF = DistributionGGX(N, H, roughness);
    
        float G = GeometrySmith(N, V, L, roughness);
    
        float3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
    
        float3 numerator = NDF * G * F;
    
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    
        float3 specular = numerator / denominator;
    
        float3 kS = F;
    
        float3 kD = float3(1.0, 1.0, 1.0) - kS;
    
        kD *= 1.0 - metallic;
    
        float NdotL = max(dot(N, L), 0.0);
    
        L2 = (kD * baseColor / PI + specular) * radiance * NdotL;
    }
        
    float shadow = ShadowCalculation(texCoord);

    float3 color = baseColor * 0.3 + L2 + (1.0 - shadow) * Lo;
    
    color = color / (color + float3(1.0, 1.0, 1.0));
    
    return float4(color, 1.0);
}