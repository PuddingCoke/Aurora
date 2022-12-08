#define MAXITERATION 150
#define LEVELOFDETAIL 0.0001

#define RAYMARCHITERATION 150

cbuffer DeltaTime : register(b0)
{
    float dt;
    float sTime;
    uint uintSeed;
    float floatSeed;
}

cbuffer SimulationParam : register(b1)
{
    float phi;
    float theta;
    float RADIUS;
    float POWER;
}

float SDF(const float3 pos)
{
    float3 z = pos;
    
    float r;
    float dr = 1.0;
    
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
    
    float3 cameraPos = float3(cos(phi) * sin(theta), cos(phi) * cos(theta), sin(phi)) * RADIUS;
    
    float3 cameraNormal = normalize(cameraPos);
    
    float3 helper = float3(0, 0, 1);
    
    float3 xVec = normalize(cross(cameraNormal, helper));
    float3 yVec = normalize(cross(cameraNormal, xVec));
    
    float3 rayDir = normalize(xVec * planePos.x + yVec * planePos.y - cameraNormal);
    float3 curPos = cameraPos;
    
    float3 color = float3(1.0, 1.0, 1.0);
    
    [loop]
    for (uint marchIteration = 0; marchIteration < RAYMARCHITERATION; marchIteration++)
    {
        float distance = SDF(curPos);
        curPos += rayDir * distance;
        color -= float3(0.008, 0.008, 0.005);
        if (distance < LEVELOFDETAIL)
            return float4(color, 1.0);
    }
    
    return float4(0.0, 0.0, 0.0, 1.0);
}