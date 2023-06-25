Texture2D<float4> gPosition : register(t0);
Texture2D<float4> colorTexture : register(t1);

SamplerState samplerState : register(s0);

cbuffer ViewInfo : register(b1)
{
    matrix view;
    float4 viewPos;
    matrix prevViewProj;
}

static const uint g_numSamples = 5;

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    float2 currentPos = float2(texCoord.x * 2.0 - 1.0, (1.0 - texCoord.y) * 2.0 - 1.0);
    
    float4 pos = gPosition.Sample(samplerState, texCoord);
    
    float4 previousPos = mul(float4(pos.xyz, 1.0), prevViewProj);
    previousPos /= previousPos.w;
    
    float2 velocity = (currentPos - previousPos.xy) / 2.0;
    
    float4 color = colorTexture.Sample(samplerState, texCoord);
    
    if (pos.w < 0.1)
    {
        return color;
    }
    
    texCoord += float2(velocity.x, -velocity.y);
    
    [unroll]
    for (uint i = 1; i < g_numSamples; ++i, texCoord += float2(velocity.x, -velocity.y))
    {
        float4 currentColor = colorTexture.Sample(samplerState, texCoord);
        color += currentColor;
    }
    
    float4 finalColor = color / float(g_numSamples);
    
    return finalColor;
}