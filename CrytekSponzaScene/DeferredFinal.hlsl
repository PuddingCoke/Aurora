Texture2D positionDepth : register(t0);
Texture2D normalSpecular : register(t1);
Texture2D albedo : register(t2);
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

cbuffer ProjMatrix : register(b1)
{
    matrix proj;
}

cbuffer ViewMatrix : register(b2)
{
    matrix view;
    matrix normalMatrix;
}

cbuffer LightInfo : register(b3)
{
    float4 viewPos;
    Light lights[17];
};

static const float AMBIENT_FACTOR = 0.2;

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    float3 position = positionDepth.Sample(linearSampler, texCoord).xyz;
    float3 normal = normalSpecular.Sample(linearSampler, texCoord).xyz;
    float specular = normalSpecular.Sample(linearSampler, texCoord).w;
    float4 color = albedo.Sample(linearSampler, texCoord);
    
    float3 ambient = color.rgb * AMBIENT_FACTOR;
    float3 outColor = ambient;
    
    if (length(position) == 0.0)
    {
        outColor = color.rgb;
    }
    else
    {
        [unroll]
        for (int i = 0; i < 17; i++)
        {
            float3 lightPos = mul(float4(lights[i].position.xyz, 1.0), view).xyz;
            float3 L = lightPos - position;
            float dist = length(L);
            L = normalize(L);
            
            float3 eyeViewSpace = mul(float4(viewPos.xyz, 1.0), view).xyz;
            float3 V = eyeViewSpace - position;
            V = normalize(V);
            
            float atten = lights[i].radius / (pow(dist, 2.0) + 1.0);
            
            float3 N = normalize(normal);
            float NdotL = max(0.0, dot(N, L));
            float3 diff = lights[i].color.rgb * color.rgb * NdotL * atten;
            
            float3 R = reflect(-L, N);
            float NdotR = max(0.0, dot(R, V));
            float3 spec = lights[i].color.rgb * specular * pow(NdotR, 16.0) * (atten * 1.5);
            
            outColor += diff + spec;
        }
        
        float ao = ssaoTexture.Sample(linearSampler, texCoord).r;
        
        outColor *= ao;
    }
    
    return float4(outColor, 1.0);
}