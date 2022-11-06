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
    
    const float rho = 0.3;
    const float ax = 0.2;
    const float ay = 0.1;

    float3 x = cross(L, N);
    float3 y = cross(x, N);

    float mult = (ONE_OVER_4PI * rho / (ax * ay * sqrt(max(1e-5, dot(L, N) * dot(V, N)))));
    float hdotx = dot(H, x) / ax;
    float hdoty = dot(H, y) / ay;
    float hdotn = dot(H, N);

    float spec = mult * exp(-((hdotx * hdotx) + (hdoty * hdoty)) / (hdotn * hdotn));
    
    float3 R = reflect(-V, N);
    
    float3 refl = skyTexture.Sample(linearSampler, R).rgb;
    
    float3 color = emissive_color * emissive_contribution +
		    ambient_color * ambient_contribution +
		    diffuse_color * diffuse_contribution * max(dot(N, L), 0) +
			specular_color * specular_contribution * (pow(max(dot(N, H), 0.0), 360.0) + spec * 0.5) + refl * 0.3;
    
    return float4(color, 1.0);
}