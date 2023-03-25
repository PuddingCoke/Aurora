RWTexture2D<uint> curState : register(u0);

cbuffer DeltaTime : register(b0)
{
    float dt;
    float sTime;
    uint uintSeed;
    float floatSeed;
}

uint hash(uint2 x)
{
    const uint m = 0x5bd1e995U;
    uint hash = uintSeed;
    // process first vector element
    uint k = x.x;
    k *= m;
    k ^= k >> 24;
    k *= m;
    hash *= m;
    hash ^= k;
    // process second vector element
    k = x.y;
    k *= m;
    k ^= k >> 24;
    k *= m;
    hash *= m;
    hash ^= k;
	// some final mixing
    hash ^= hash >> 13;
    hash *= m;
    hash ^= hash >> 15;
    return hash;
}

[numthreads(32, 18, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint hashValue = hash(DTid.xy);
    float value = float(hashValue) / float(0xffffffffU);
    
    if(value>0.5)
    {
        curState[DTid.xy] = 255;
    }
    else
    {
        curState[DTid.xy] = 0;
    }
}