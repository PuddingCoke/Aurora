struct PixelInput
{
    float3 position : POSITION;
    float2 texCoord : TEXCOORD;
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

SamplerState linearSampler : register(s0);

static const float3 L = normalize(float3(0.0, 1.0, -1.0));
static const float3 oceanColor = float3(0.0000, 0.3307, 0.3613);

float4 main(PixelInput input) : SV_TARGET
{
    float4 NJ = normalTexture.Sample(linearSampler, input.texCoord);
    
    float3 N = normalize(NJ.xyz);
    
    float3 V = normalize(viewPos.xyz - input.position.xyz);
    
    float F0 = 0.020018673;
    
    float F = F0 + (1.0 - F0) * pow(saturate(1.0 - dot(N, V)), 5.0);
    
    float3 R = normalize(reflect(-V, N));
    
    if (R.y < 0.0)
    {
        R = -R;
    }
    
    float3 H = normalize(V + L);
    
    float3 reflectColor = skyTexture.Sample(linearSampler, R).rgb;
    
    float turbulence = max(1.6 - NJ.w, 0.0);
    
    float highlightMul = 1.0 + 2.0 * smoothstep(1.2, 1.8, turbulence);
    
    float spec = pow(max(dot(N, H), 0.0), 3072.0);
    
    float3 color = oceanColor + spec * float3(1.0, 1.0, 1.0);
    
    color = lerp(color, reflectColor * highlightMul, F);
    
    float fogFactor = distance(viewPos.xz, input.position.xz);
    
    float2 fogTexcoord = normalize(input.position.xz - viewPos.xz);
    
    float3 fogColor = skyTexture.Sample(linearSampler, float3(fogTexcoord.x, 0.0, fogTexcoord.y)).rgb;
    
    fogFactor = pow(smoothstep(0.0, 1536.0, fogFactor), 0.5);
    
    return float4(lerp(color, fogColor, fogFactor), 1.0);
}