Texture2D<float4> originTexture : register(t0);
SamplerState samplerState : register(s0);

cbuffer BloomParam : register(b1)
{
    float exposure;
    float gamma;
    float threshold;
    float intensity;
    float softThreshold;
    float3 padding;
};

float4 main(float2 texCoord : TEXCOORD) : SV_Target
{
    const float3 color = originTexture.Sample(samplerState, texCoord).rgb;
    
    float brightness = max(color.r, max(color.g, color.b));
    
    float knee = threshold * softThreshold;
    
    float soft = brightness - threshold + knee;
    
    soft = clamp(soft, 0.0, 2.0 * knee);
    
    soft = soft * soft / (4.0 * knee + 0.00001);
    
    float contribution = max(soft, brightness - threshold);
    
    contribution /= max(brightness, 0.00001);
    
    return float4(color * contribution, 1.0);
}