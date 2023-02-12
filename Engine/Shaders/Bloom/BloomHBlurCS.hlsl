cbuffer BlurParams : register(b1)
{
    float4 weights[2];
    float4 offsets[2];
    float2 texelSize;
    uint iteration;
    float sigma;
};

Texture2D<float4> sourceTexture : register(t0);

RWTexture2D<float4> destTexture : register(u0);

SamplerState linearSampler : register(s0);

static float weight[8] = (float[8]) weights;
static float offset[8] = (float[8]) offsets;

[numthreads(60, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    const float2 texCoord = (float2(DTid.xy) + 0.5) * texelSize;
    
    float3 result = sourceTexture.SampleLevel(linearSampler, texCoord, 0.0).rgb * weight[0];
    
    [unroll]
    for (uint i = 1; i < iteration; ++i)
    {
        result += sourceTexture.SampleLevel(linearSampler, texCoord + float2(texelSize.x * offset[i], 0.0), 0.0).rgb * weight[i];
        result += sourceTexture.SampleLevel(linearSampler, texCoord - float2(texelSize.x * offset[i], 0.0), 0.0).rgb * weight[i];
    }
    
    destTexture[DTid.xy] = float4(result, 1.0);
}