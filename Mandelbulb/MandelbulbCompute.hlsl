RWTexture3D<float4> tTexture : register(u0);

cbuffer Parameters : register(b1)
{
    float dimension;
    float scale;
};

#define POWER 8.0

//v = v^8 + c
//float3 iteration(float3 pos)
//{
//    float r = length(pos);
//    float theta = atan2(length(pos.xy), pos.z);
//    float phi = atan2(pos.y, pos.x);
    
//    float newx = pow(r, POWER) * sin(theta * POWER) * cos(phi * POWER);
//    float newy = pow(r, POWER) * sin(theta * POWER) * sin(phi * POWER);
//    float newz = pow(r, POWER) * cos(theta * POWER);
    
    
//}

[numthreads(8, 8, 8)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    //float3 pos = (float3(DTid) - dimension / 2.0) * scale;
    float3 color = float3(DTid) / 256.0;
    tTexture[DTid] = float4(color, 1.0);
}