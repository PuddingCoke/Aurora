cbuffer BloomParam : register(b1)
{
    float exposure;
    float gamma;
    float threshold;
    float intensity;
};

struct BlurParams
{
    float weight[4];
    float offset[4];
    float2 texelSize;
    uint iteration;
    float v0;
};

Texture2D<float4> sourceTexture : register(t0);
StructuredBuffer<BlurParams> blurParams : register(t1);

RWTexture2D<float4> destTexture : register(u0);

SamplerState linearSampler : register(s0);

[numthreads(60, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    const BlurParams param = blurParams.Load(0);
    
    const float2 texCoord = (float2(DTid.xy) + 0.5) * param.texelSize;
    
    float3 result = sourceTexture.SampleLevel(linearSampler, texCoord, 0.0).rgb * param.weight[0];
    
    [unroll]
    for (uint i = 1; i < param.iteration; ++i)
    {
        result += sourceTexture.SampleLevel(linearSampler, texCoord + float2(param.texelSize.x * param.offset[i], 0.0), 0.0).rgb * param.weight[i];
        result += sourceTexture.SampleLevel(linearSampler, texCoord - float2(param.texelSize.x * param.offset[i], 0.0), 0.0).rgb * param.weight[i];
    }
    
    destTexture[DTid.xy] = float4(intensity * result, 1.0);
}