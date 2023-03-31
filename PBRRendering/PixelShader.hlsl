struct PixelInput
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
};

cbuffer ViewMatrix : register(b1)
{
    matrix view;
    float4 viewPos;
}

cbuffer Material : register(b2)
{
    float4 ambientColor;
    float4 diffuseColor;
    float4 specularColor;
}

cbuffer Light : register(b3)
{
    float4 lightPos;
    float4 lightColor;
}

Texture2D<float2> brdfLookup : register(t0);
TextureCube irradianceCube : register(t1);
TextureCube prefilterCube : register(t2);

SamplerState linearSampler : register(s0);

#define PI 3.14159265358979323846264

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

static const float metallic = 0.5;
static const float roughness = 0.1;

float4 main(PixelInput input) : SV_TARGET
{
    float3 albedo = pow(diffuseColor.rgb, 2.2);
    
    float3 N = normalize(input.normal);
    float3 V = normalize(viewPos.xyz - input.pos);
    float3 L = normalize(lightPos.xyz - input.pos);
    float3 H = normalize(V + L);
    float3 R = reflect(-V, N);
    
    float3 F0 = float3(0.04, 0.04, 0.04);
    F0 = lerp(F0, albedo, metallic);
    
    float distance = length(lightPos.xyz - input.pos);
    float atten = 1.0 / (distance * distance);
    float3 radiance = float3(8.0, 8.0, 8.0) * atten;
    
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
    
    float3 color = (kD * albedo / PI + specular) * radiance * NdotL;
    
    F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    
    kS = F;
    
    kD = float3(1.0, 1.0, 1.0) - kS;
    kD *= 1.0 - metallic;
    
    float3 irradiance = irradianceCube.Sample(linearSampler, N).rgb;
    float3 diffuse = irradiance * albedo;
    
    const float MAX_REFLECTION_LOD = 9.0;
    
    float3 prefilteredColor = prefilterCube.SampleLevel(linearSampler, R, roughness * MAX_REFLECTION_LOD).rgb;
    float2 brdf = brdfLookup.Sample(linearSampler, float2(max(dot(N, V), 0.0), roughness)).rg;
    specular = prefilteredColor * (F * brdf.x + brdf.y);
    
    float3 ambient = kD * diffuse + specular;
    
    color += ambient;
    
    color = color / (color + 1.0);
    
    color = pow(color, float3(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2));
    
    return float4(color, 1.0);
}