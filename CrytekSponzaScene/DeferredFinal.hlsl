Texture2D gPosition : register(t0);
Texture2D gNormalSpecular : register(t1);
Texture2D gBaseColor : register(t2);
Texture2D ssaoTexture : register(t3);

SamplerState wrapSampler : register(s0);
SamplerState linearSampler : register(s1);

struct Light
{
    float4 position;
    float4 color;
    float radius;
    float quadraticFalloff;
    float linearFalloff;
    float v0;
};

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
    Light lights[17];
};

static const float AMBIENT_FACTOR = 0.15;

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    const float3 position = gPosition.Sample(linearSampler, texCoord).xyz;
    const float4 normalSpecular = gNormalSpecular.Sample(linearSampler, texCoord);
    const float3 baseColor = gBaseColor.Sample(linearSampler, texCoord).rgb;
    
    float3 outColor = baseColor * AMBIENT_FACTOR;
    
    [unroll]
    for (int i = 0; i < 17; i++)
    {
        float3 L = lights[i].position.xyz - position;
        const float dist = length(L);
        L = normalize(L);
            
        const float3 V = normalize(viewPos.xyz - position);
            
        const float atten = lights[i].radius / (pow(dist, 2.0) + 1.0);
            
        const float3 N = normalize(normalSpecular.rgb);
        const float NdotL = max(0.0, dot(N, L));
        const float3 diff = lights[i].color.rgb * baseColor.rgb * NdotL * atten;
            
        const float3 R = reflect(-L, N);
        const float NdotR = max(0.0, dot(R, V));
        const float3 spec = lights[i].color.rgb * normalSpecular.w * pow(NdotR, 16.0) * (atten * 1.5);
        
        outColor += diff + spec;
    }
        
    const float ao = ssaoTexture.Sample(linearSampler, texCoord).r;
        
    outColor *= ao;
    
    return float4(outColor, 1.0);
}