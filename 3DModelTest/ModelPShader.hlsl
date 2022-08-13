struct PixelInput
{
    float3 position : POSITION;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
};

cbuffer DeltaTimes : register(b0)
{
    float deltaTime;
    float sTime;
    float2 v0;
}

cbuffer MaterialProperty : register(b1)
{
    float3 ambientColor;
    float shininess;
    float3 diffuseColor;
    float refraction;
    float3 specularColor;
    float opacity;
    float3 emmisiveColor;
    float v3;
}

cbuffer LightInfo : register(b2)
{
    float3 lightPos;
    float v4;
    float3 viewPos;
    float v5;
};

static const float PI = 3.14159265359;

SamplerState linearSampler : register(s0);

TextureCube irradianceMap : register(t0);
TextureCube prefilterMap : register(t1);
Texture2D brdfLUT : register(t2);

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

float4 main(PixelInput input) : SV_TARGET
{
    const float3 albedo = pow(diffuseColor, 2.2);
    const float metallic = 0.5;
    const float roughness = 0.1;
    
    float3 N = input.normal;
    float3 V = normalize(viewPos - input.position);
    float3 R = reflect(-V, N);
    
    float specularF0 = 0.3;
    
    float3 F0 = float3(specularF0, specularF0, specularF0);
    F0 = lerp(F0, albedo, 0.0);
    
    float3 Lo = float3(0.0, 0.0, 0.0);
    
    float3 L = normalize(lightPos - input.position);
    float3 H = normalize(V + L);
    
    float distance = length(lightPos - input.position);
    float attenuation = 1.0 / (distance * distance);
    
    const float3 lightColor = float3(8.0, 8.0, 8.0);
    
    float3 radiance = lightColor * attenuation;
    
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
    
    Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    
    kS = fresnelSchlick(max(dot(N, V), 0.0), F0);
    
    kD = 1.0 - kS;
    
    kD *= 1.0 - metallic;
    
    float3 irradiance = irradianceMap.Sample(linearSampler, N).rgb;
    
    float3 diffuse = irradiance * albedo;
    
    const float MAX_LOD = 4.0;
    
    float3 prefilteredColor = prefilterMap.SampleLevel(linearSampler, R, roughness * MAX_LOD).rgb;
    
    float2 brdf = brdfLUT.Sample(linearSampler, float2(max(dot(N, V), 0.0), roughness)).rg;
    
    specular = prefilteredColor * (F * brdf.x + brdf.y);
    
    float3 ambient = (kD * diffuse + specular);
    
    float3 color = Lo + ambient;
    
    color = color / (color + float3(1.0, 1.0, 1.0));
    
    const float gamma = 2.2;
    
    color = pow(color, float3(1.0 / gamma, 1.0 / gamma, 1.0 / gamma));
    
    return float4(color, 1.0);
}