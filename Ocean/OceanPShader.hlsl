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

SamplerState linearSampler : register(s0);

#define ONE_OVER_4PI 0.0795774715459476

static const float3 L = normalize(float3(1.0, 1.0, 1.0));
static const float3 oceanColor = float3(0.0000, 0.2307, 0.3613);
static const float3 sunColor = float3(1.0, 1.0, 1.0);
static const float3 perlinFrequency = float3(1.12, 0.59, 0.23);
static const float3 perlinGradient = float3(0.014, 0.016, 0.022);

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

    float3 h = L + V;
    float3 x = cross(L, N);
    float3 y = cross(x, N);

    float mult = (ONE_OVER_4PI * rho / (ax * ay * sqrt(max(1e-5, dot(L, N) * dot(V, N)))));
    float hdotx = dot(h, x) / ax;
    float hdoty = dot(h, y) / ay;
    float hdotn = dot(h, N);

    float spec = mult * exp(-((hdotx * hdotx) + (hdoty * hdoty)) / (hdotn * hdotn));
    
    float3 color = lerp(oceanColor, refl * color_mod, F) + sunColor * spec;

    return float4(color, 1.0);
}