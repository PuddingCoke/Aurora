struct PixelInput
{
    float3 position : POSITION;
    float2 texCoord : TEXCOORD0;
    float2 patchTexCoord : TEXCOORD1;
};

cbuffer DeltaTime : register(b0)
{
    float dt;
    float sTime;
    uint uintSeed;
    float floatSeed;
}

cbuffer ViewMatrix : register(b1)
{
    matrix view;
    float4 viewPos;
}

Texture2D normalTexture : register(t0);
TextureCube skyTexture : register(t1);
Texture2D prelinTexture : register(t2);
Texture2D foamBubbleTexture : register(t3);
Texture2D foamIntensityTexture : register(t4);
Texture2D windGustsTexture : register(t5);

SamplerState linearSampler : register(s0);

#define ONE_OVER_4PI 0.0795774715459476

static const float3 L = normalize(float3(1.0, 1.0, 1.0));
static const float3 oceanColor = float3(0.0000, 0.3307, 0.3613);
static const float3 sunColor = float3(1.0, 1.0, 1.0);
static const float3 perlinFrequency = float3(1.12, 0.59, 0.23);
static const float3 perlinGradient = float3(0.014, 0.016, 0.022);

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

float4 main(PixelInput input) : SV_TARGET
{
    float dist = length((viewPos.xyz - input.position.xyz).xz);
    float factor = (2048.0 - dist) / 2048.0;
    float2 perl = float2(0.0, 0.0);
    
    factor = saturate(factor * factor * factor);
    
    float2 perlinOffset = float2(-0.06 * sTime, 0.0);
    
    if (factor < 1.0)
    {
        float2 ptex = input.texCoord + input.patchTexCoord;
        
        float2 p0 = prelinTexture.Sample(linearSampler, ptex * perlinFrequency.x + perlinOffset).rg;
        float2 p1 = prelinTexture.Sample(linearSampler, ptex * perlinFrequency.y + perlinOffset).rg;
        float2 p2 = prelinTexture.Sample(linearSampler, ptex * perlinFrequency.z + perlinOffset).rg;

        perl = (p0 * perlinGradient.x + p1 * perlinGradient.y + p2 * perlinGradient.z);
    }
    
    float4 NJ = normalTexture.Sample(linearSampler, input.texCoord);
    
    NJ.xz = lerp(perl, NJ.xz, factor);
    
    float3 N = normalize(NJ.xyz);
    float3 V = normalize(viewPos.xyz - input.position.xyz);
    float3 R = reflect(-V, N);
    
    float F0 = 0.020018673;
    float F = F0 + (1.0 - F0) * pow(1.0 - dot(N, R), 5.0);
    
    float3 refl = skyTexture.Sample(linearSampler, R).rgb;
    
    float turbulence = max(1.6 - NJ.w, 0.0);
    
    float color_mod = lerp(1.0, 1.0 + 3.0 * smoothstep(1.2, 1.8, turbulence), factor);
    
    float rho = 0.3;
    float ax = 0.2;
    float ay = 0.1;

    float3 h = normalize(L + V);
    float3 x = cross(L, N);
    float3 y = cross(x, N);

    float mult = (ONE_OVER_4PI * rho / (ax * ay * sqrt(max(1e-5, dot(L, N) * dot(V, N)))));
    float hdotx = dot(h, x) / ax;
    float hdoty = dot(h, y) / ay;
    float hdotn = dot(h, N);

    float spec = mult * exp(-((hdotx * hdotx) + (hdoty * hdoty)) / (hdotn * hdotn));
    
    float jacobian = NJ.w;
    
    jacobian = min(1, max(0, (-jacobian + 1.0) * 1.0));
    
    float3 foamColor = foamBubbleTexture.Sample(linearSampler, input.texCoord + input.patchTexCoord).rgb * foamIntensityTexture.Sample(linearSampler, input.texCoord + input.patchTexCoord).rgb;
    
    float3 albedo = lerp(0.0, foamColor, jacobian);
    
    float roughness = 0.0;
    
    float metallic = 0.0;
    
    float distanceGloss = lerp(1 - 0.0, 0.0, 1 / (1 + length(viewPos.xyz - input.position.xyz) * 0.1));
    
    roughness = 1.0 - lerp(distanceGloss, 0, jacobian);
    
    float3 radiance = float3(2.0, 2.0, 2.0);
    
    float NDF = DistributionGGX(N, h, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    
    float3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    float3 specular = numerator / denominator;
    
    float3 kS = F;
    
    float3 kD = float3(1.0, 1.0, 1.0) - kS;
    
    kD *= 1.0 - metallic;
    
    float NdotL = max(dot(N, L), 0.0);
    
    float3 foam = (kD * albedo / PI + specular) * radiance * NdotL * factor;
    
    float3 color = lerp(oceanColor, refl * color_mod, F) + sunColor * spec;
    
    return float4(color+foam, 1.0);
}