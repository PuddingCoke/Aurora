Texture3D<float4> mandelTexture : register(t0);
Texture2D<float4> texFrontTexture : register(t1);
Texture2D<float4> texBackTexture : register(t2);

SamplerState borderSampler : register(s0);

static const uint maxIteration = 256;

static const float stepSize = sqrt(3.0) / maxIteration;

float4 main(float4 pos : SV_Position) : SV_TARGET
{
    float2 tex = pos.xy / float2(1920.0, 1080.0);
    
    float3 frontPos = texFrontTexture.Sample(borderSampler, tex).xyz;
    float3 backPos = texBackTexture.Sample(borderSampler, tex).xyz;
    
    float3 dir = normalize(backPos - frontPos);
    
    float3 step = stepSize * dir;
    
    float3 v = frontPos;
    
    float4 dst = float4(0.0, 0.0, 0.0, 0.0);
    
    [unroll]
    for (uint i = 0; i < maxIteration; i++)
    {
        
        float4 src = mandelTexture.Sample(borderSampler, v);
        
        dst.rgb = dst.rgb + (1.0 - dst.a) * src.a * src.rgb;
        dst.a = dst.a + (1.0 - dst.a) * src.a;
        
        if (dst.a >= 0.95)
            break;
        
        v += step;
        
        if (v.x > 1.0 || v.y > 1.0 || v.z > 1.0)
            break;
    }
    
    return dst;
}