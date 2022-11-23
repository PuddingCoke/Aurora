struct PixelInput
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
};

cbuffer ViewMatrix : register(b1)
{
    matrix view;
    float4 viewPos;
}

#define ONE_OVER_4PI	0.0795774715459476

Texture2D normalTexture : register(t0);
TextureCube skyTexture : register(t1);

SamplerState linearSampler : register(s0);

float2 ClampUV(float2 uv)
{
    return max(0.0, min(1.0, uv));
}

float4 main(PixelInput input) : SV_TARGET
{
    float3 N = normalize(normalTexture.Sample(linearSampler, input.uv).xyz);
    
    float3 L = normalize(float3(1.0, 1.0, 0.0));
    
    float3 V = normalize(viewPos.xyz - input.position);
    
    float3 H = normalize(L + V);
    
    float3 emissive_color = float3(1.0, 1.0, 1.0);
    float3 ambient_color = float3(0.0, 0.65, 0.75);
    float3 diffuse_color = float3(0.5, 0.65, 1.0);
    float3 specular_color = float3(1.0, 1.0, 1.0);

    float emissive_contribution = 0.10;
    float ambient_contribution = 0.30;
    float diffuse_contribution = 0.30;
    float specular_contribution = 0.70;
    
    float3 R = reflect(-V, N);
    
    float3 refl = skyTexture.Sample(linearSampler, R).rgb;
    
    float3 color = emissive_color * emissive_contribution +
		    ambient_color * ambient_contribution +
		    diffuse_color * diffuse_contribution * max(dot(N, L), 0) +
			specular_color * specular_contribution * (pow(max(dot(R, H), 0.0), 64.0)) + refl * 0.3;
    
    return float4(color, 1.0);
}