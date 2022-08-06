struct PixelInput
{
    float3 position : POSITION;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
};

cbuffer DeltaTimes : register(b0)
{
    float deltaTime;
    float sTime;
    float2 v0;
}

cbuffer MaterialProperty : register(b1)
{
    float3 ambientColor;
    float shininess;
    float3 diffuseColor;
    float refraction;
    float3 specularColor;
    float opacity;
    float3 emmisiveColor;
    float v3;
}

cbuffer LightInfo : register(b2)
{
    float3 lightPos;
    float v4;
    float3 viewPos;
    float v5;
};

float4 main(PixelInput input) : SV_TARGET
{
    float3 ambient = 0.05 * ambientColor;
    
    float3 lightDir = normalize(lightPos - input.position);
    
    float diff = max(dot(lightDir, input.normal), 0.0);
    
    float3 diffuse = diff * diffuseColor;
    
    float3 viewDir = normalize(viewPos - input.position);
    
    float3 reflectDir = reflect(-lightDir, input.normal);
    
    float3 halfwayDir = normalize(lightDir + viewDir);
    
    float spec = pow(max(dot(input.normal, halfwayDir), 0.0), shininess);
    
    float3 specular = spec * specularColor;
    
    return float4(ambient + diffuse + specular, 1.0);
}