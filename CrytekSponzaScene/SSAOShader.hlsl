Texture2D positionDepth : register(t0);
Texture2D normalSpecular : register(t1);
Texture2D randomTexture : register(t2);

SamplerState wrapSampler : register(s0);
SamplerState linearSampler : register(s1);

static const float random_size = 64.0;
static const float2 g_screen_size = { 1920.0, 1080.0 };
static const float g_sample_rad = 1.5;
static const float g_intensity = 2.5;
static const float g_scale = 1.0;
static const float g_bias = 0.025;

float3 getPosition(in float2 uv)
{
    return positionDepth.Sample(linearSampler, uv).xyz;
}

float3 getNormal(in float2 uv)
{
    return normalSpecular.Sample(linearSampler, uv).xyz;
}

float2 getRandom(in float2 uv)
{
    return normalize(randomTexture.Sample(wrapSampler, g_screen_size * uv / random_size).xy * 2.0f - 1.0f);
}

float doAmbientOcclusion(in float2 tcoord, in float2 uv, in float3 p, in float3 cnorm)
{
    float3 diff = getPosition(tcoord + uv) - p;
    const float3 v = normalize(diff);
    const float d = length(diff) * g_scale;
    return max(0.0, dot(cnorm, v) - g_bias) * (1.0 / (1.0 + d)) * g_intensity;
}

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    const float2 vec[4] = { float2(1, 0), float2(-1, 0), float2(0, 1), float2(0, -1) };
    float3 p = getPosition(texCoord);
    float3 n = getNormal(texCoord);
    float2 rand = getRandom(texCoord);
    float ao = 0.0f;
    float rad = g_sample_rad / p.z;
  
    int iterations = lerp(6.0, 2.0, p.z / 1000.0);
    for (int j = 0; j < iterations; ++j)
    {
        float2 coord1 = reflect(vec[j], rand) * rad;
        float2 coord2 = float2(coord1.x * 0.707 - coord1.y * 0.707, coord1.x * 0.707 + coord1.y * 0.707);
    
        ao += doAmbientOcclusion(texCoord, coord1 * 0.25, p, n);
        ao += doAmbientOcclusion(texCoord, coord2 * 0.5, p, n);
        ao += doAmbientOcclusion(texCoord, coord1 * 0.75, p, n);
        ao += doAmbientOcclusion(texCoord, coord2, p, n);
    }
  
    ao /= (float) iterations * 4.0;
    
    ao = 1.0 - ao;
    
    return float4(ao, ao, ao, 1.0);
}