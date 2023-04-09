Texture2D reflectedUV : register(t0);
Texture2D originTexture : register(t1);
Texture2D gNormalSpecular : register(t2);

SamplerState wrapSampler : register(s0);
SamplerState clampSampler : register(s1);
SamplerComparisonState shadowSampler : register(s2);

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    int size = 1;
    float separation = 1.0;
	
    float4 uv = reflectedUV.Sample(clampSampler, texCoord);
    
    //if (uv.b < 0.5)
    //{
    //    uv = float4(0.0, 0.0, 0.0, 0.0);
        
    //    float count = 0.0;
        
    //    [unroll]
    //    for (int i = -size; i <= size; ++i)
    //    {
    //        [unroll]
    //        for (int j = -size; j <= size; ++j)
    //        {
    //            uv += reflectedUV.SampleLevel(clampSampler, texCoord + float2(i, j) * separation / float2(1920.0, 1080.0), 0.0);
    //            count += 1.0;
    //        }
    //    }
        
    //    if (count > 0.0)
    //    {
    //        uv /= count;
    //    }
    //}
    
    float3 color = originTexture.Sample(clampSampler, uv.xy).rgb;
    
    float alpha = clamp(uv.b, 0.0, 1.0);
    
    color = lerp(float3(0.0, 0.0, 0.0), color, alpha) * gNormalSpecular.Sample(clampSampler, texCoord).a;
    
    color += originTexture.Sample(clampSampler, texCoord).rgb;
    
    return float4(color, 1.0);
}