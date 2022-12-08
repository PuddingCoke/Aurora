#define RADIUS 1.65
#define RAYMARCHITERATION 150

cbuffer DeltaTime : register(b0)
{
    float dt;
    float sTime;
    uint uintSeed;
    float floatSeed;
}

float SDF(const float3 pos)
{
    return length(pos);
}

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    float2 planePos = texCoord * 2.0 - 1.0;
    planePos.x *= 16.0 / 9.0;
    
    float theta = 0.25 * sTime;
    float phi = 0.5 * sTime;
    
    float3 cameraPos = float3(sin(phi) * sin(theta), sin(phi) * cos(theta), cos(phi)) * RADIUS;
    
    float3 cameraNormal = normalize(cameraPos);
    
    float3 helper = abs(cameraNormal.x) > 0.99 ? float3(0, 0, 1) : float3(1, 0, 0);
    
    float3 yVec = normalize(cross(cameraNormal, helper));
    float3 xVec = normalize(cross(cameraNormal, yVec));
    
    float3 rayDir = normalize(xVec * planePos.x + yVec * planePos.y - cameraNormal);
    float3 curPos = cameraPos;
    
    [loop]
    for (uint marchIteration = 0; marchIteration < RAYMARCHITERATION; marchIteration++)
    {
        float distance = SDF(curPos);
        curPos += rayDir * distance;
        if (distance < 1.0)
            return float4(sin(curPos) * 0.5 + 0.5, 1.0);
    }
    
    return float4(0.0, 0.0, 0.0, 1.0);
}