cbuffer DeltaTime : register(b0)
{
    float deltaTime;
    float sumTime;
    uint uintSeed;
    float floatSeed;
}

//https://www.shadertoy.com/view/XlXcW4

float3 hash(uint3 x)
{
    x = ((x >> 8U) ^ x.yzx) * 1103515245U;
    x = ((x >> 8U) ^ x.yzx) * 1103515245U;
    x = ((x >> 8U) ^ x.yzx) * 1103515245U;
    
    return float3(x) * (1.0 / float(0xffffffffU));
}

RWTexture2D<float4> noiseTexture : register(u0);

[numthreads(32, 18, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    noiseTexture[DTid.xy] = float4(hash(uint3(DTid.xy, uintSeed)), 1.0);
}