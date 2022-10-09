struct PixelInput
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
};

cbuffer ViewMatrix : register(b1)
{
    matrix view;
    matrix normalMatrix;
    float4 viewPos;
}

Texture2D normalTexture : register(t0);

SamplerState linearSampler : register(s0);

static const float3 sun = float3(1.0, 1.0, 0.0);

float4 main(PixelInput input) : SV_TARGET
{
    float3 N = normalize(normalTexture.Sample(linearSampler, input.uv).xyz);
    
    float3 L = normalize(sun);
    
    float3 V = normalize(viewPos.xyz - input.position);
    
    float3 H = normalize(L + V);
    
    float4 emissive_color = float4(1.0, 1.0, 1.0, 1.0);
    float4 ambient_color = float4(0.0, 0.65, 0.75, 1.0);
    float4 diffuse_color = float4(0.5, 0.65, 0.75, 1.0);
    float4 specular_color = float4(1.0, 1.0, 1.0, 1.0);

    float emissive_contribution = 0.10;
    float ambient_contribution = 0.30;
    float diffuse_contribution = 0.30;
    float specular_contribution = 0.70;
 
    
    float4 c = float4(1.0, 1.0, 1.0, 1.0);
    
    float4 color = emissive_color * emissive_contribution +
		    ambient_color * ambient_contribution * c +
		    diffuse_color * diffuse_contribution * c * max(dot(N, L), 0) +
			specular_color * specular_contribution * c * max(pow(dot(N, H), 120.0), 0.0);
    
    color.a = 1.0;
    
    return color;
}