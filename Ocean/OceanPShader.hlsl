struct PixelInput
{
    float3 position : POSITION0;
    float3 positionUndisplaced : POSITION1;
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

static const float3 L = normalize(float3(1.0, 1.0, 1.0));
static const float3 oceanColor = float3(0.0000, 0.3307, 0.3613);
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
    float3 R = normalize(reflect(-V, N));
    float3 H = normalize(V + R);
    
    float F0 = 0.020018673;
    float F = F0 + (1.0 - F0) * pow(saturate(1.0 - dot(H, V)), 5.0);
    
    float3 reflectColor = skyTexture.Sample(linearSampler, R).rgb;
    
    float turbulence = max(1.6 - NJ.w, 0.0);
    
    //add more highlight when surface is steep.
    //and also blend this with factor.when distance is far highlight should not be visible. 
    float highlightMul = lerp(1.0, 1.0 + 3.0 * smoothstep(1.2, 1.8, turbulence), factor);
    
    float3 color = lerp(oceanColor, reflectColor * highlightMul, F);
    
    return float4(color, 1.0);
}