RWTexture3D<float4> tTexture : register(u0);

#define POWER 8.0
#define MAXITERATION 400
#define SCALE 2.15

cbuffer DeltaTime : register(b0)
{
    float deltaTime;
    float sumTime;
}

//v = v^8 + c
float3 iteration(float3 pos, float3 origin)
{
    float r = length(pos);
    float theta = atan2(length(pos.xy), pos.z);
    float phi = atan2(pos.y, pos.x);
    
    float newx = pow(r, POWER) * sin(theta * POWER) * cos(phi * POWER);
    float newy = pow(r, POWER) * sin(theta * POWER) * sin(phi * POWER);
    float newz = pow(r, POWER) * cos(theta * POWER);
    
    return float3(newx + origin.x, newy + origin.y, newz + origin.z);
}


[numthreads(8, 8, 8)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    tTexture[DTid] = float4(0.0, 0.0, 0.0, 0.0);
    
    const float3 pos = SCALE * float3(DTid) / 1000.0 - SCALE / 2.0;
    
    float3 c = pos;
    
    uint flag = 1;
    
    [loop]
    for (uint i = 0; i < MAXITERATION; i++)
    {
        c = iteration(c, pos);
        
        if (dot(c, c) > 2.0)
        {
            flag = 0;
            break;
        }
    }
    
    if (flag)
    {
        tTexture[DTid] = float4((0.5 * sin(pos) + 0.5), 1.0);
    }
}