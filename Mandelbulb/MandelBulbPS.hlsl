#define MAXITERATION 150
#define RAYMARCHITERATION 150

cbuffer DeltaTime : register(b0)
{
    float dt;
    float sTime;
    uint uintSeed;
    float floatSeed;
}

//v = v^8 + c
float SDF(const float3 pos)
{
    float3 z = pos;
    
    float r;
    float dr = 1.0;
    
    float POWER = 6.0 + 2.0 * sin(0.25 * sTime);
    
    [loop]
    for (uint i = 0; i < MAXITERATION; i++)
    {
        r = length(z);
        
        if (r > 2.0)
            break;
        
        const float theta = asin(z.z / r) * POWER;
        const float phi = atan2(z.y, z.x) * POWER;
        
        dr = pow(r, POWER - 1.0) * POWER * dr + 1.0;
        const float zr = pow(r, POWER);
        
        z = float3(sin(theta) * cos(phi), sin(phi) * sin(theta), cos(theta)) * zr + pos;
    }
    
    return log(r) * r / dr * 0.5;
}

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    float2 planePos = texCoord * 2.0 - 1.0;
    planePos.x *= 16.0 / 9.0;
    
    float3 cameraPos = float3(0.0, sin(0.5 * sTime) * 1.65, cos(0.5 * sTime) * 1.65);
    float3 cameraNormal = normalize(cameraPos);
    float3 rayDir = normalize(float3(0.0, -cameraNormal.z, cameraNormal.y) * planePos.x + float3(planePos.y, 0.0, 0.0) - cameraNormal);
    float3 curPos = cameraPos;
    
    float3 color = float3(1.0, 1.0, 1.0);
    
    [loop]
    for (uint march_iteration = 0; march_iteration < RAYMARCHITERATION; march_iteration++)
    {
        float distance = SDF(curPos);
        curPos += rayDir * distance;
        color -= float3(0.008, 0.008, 0.005);
        if (distance < 0.0001)
            return float4(color, 1.0);
    }
    
    return float4(0.0, 0.0, 0.0, 1.0);
}