SamplerState linearSampler : register(s0);

Texture2D accTexture : register(t0);

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    float3 color = accTexture.Sample(linearSampler, texCoord).rgb;
    
    color *= 200.0;
    
    //Tonemapping and color grading
    color = pow(color, float3(1.5, 1.5, 1.5));
    color = color / (1.0 + color);
    color = pow(color, float3(1.0 / 1.5, 1.0 / 1.5, 1.0 / 1.5));

    
    color = lerp(color, color * color * (3.0 - 2.0 * color), float3(1.0, 1.0, 1.0));
    color = pow(color, float3(1.3, 1.20, 1.0));

    color = saturate(color * 1.01);
    
    color = pow(color, float3(0.7 / 2.2, 0.7 / 2.2, 0.7 / 2.2));
    
    return float4(color, 1.0);
}