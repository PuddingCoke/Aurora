Texture2D gPosition : register(t0);
Texture2D gNormalSpecular : register(t1);
Texture2D gBaseColor : register(t2);
Texture2D ssaoTexture : register(t3);
Texture2D shadowTexture : register(t4);

SamplerState wrapSampler : register(s0);
SamplerState linearSampler : register(s1);

cbuffer DeltaTimes : register(b0)
{
    float deltaTime;
    float sTime;
    float2 v0;
}

cbuffer ViewMatrix : register(b1)
{
    matrix view;
    matrix normalMatrix;
    float4 viewPos;
}

cbuffer LightInfo : register(b2)
{
    float4 lightDir;
    float4 lightColor;
};

#define AMBIENT_FACTOR 0.5

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    const float3 position = gPosition.Sample(linearSampler, texCoord).xyz;
    const float4 normalSpecular = gNormalSpecular.Sample(linearSampler, texCoord);
    const float3 baseColor = gBaseColor.Sample(linearSampler, texCoord).rgb;
    const float ao = ssaoTexture.Sample(linearSampler, texCoord).r;
    const float shadow = shadowTexture.Sample(linearSampler, texCoord).r;
    
    const float3 L = lightDir.xyz;
    
    const float3 V = normalize(viewPos.xyz - position);
        
    const float3 N = normalize(normalSpecular.rgb);
    const float NdotL = max(0.0, dot(N, L));
    const float3 diff = lightColor.rgb * baseColor.rgb * NdotL;
        
    const float3 R = reflect(-L, N);
    const float NdotR = max(0.0, dot(R, V));
    const float3 spec = lightColor.rgb * normalSpecular.w * pow(NdotR, 16.0) * 1.5;
    
    const float3 outColor = (baseColor * AMBIENT_FACTOR + (diff + spec) * shadow) * ao;
    
    return float4(outColor, 1.0);
}