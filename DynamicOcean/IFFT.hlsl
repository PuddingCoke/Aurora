#define TWO_PI 6.283185307179586476925286766559


cbuffer OceanParam : register(b1)
{
    uint mapResolution;
    float mapLength;
    float2 wind;
    float amplitude;
    float gravity;
    uint log2MapResolution;
    float v0;
};

float2 ComplexMul(float2 z, float2 w)
{
    return float2(z.x * w.x - z.y * w.y, z.y * w.x + z.x * w.y);
}

Texture2D<float2> input : register(t0);
RWTexture2D<float2> output : register(u0);

groupshared float2 pingpong[2][SIMRES];

[numthreads(SIMRES, 1, 1)]
void main(uint3 groupThreadID : SV_GroupThreadID, uint3 groupID : SV_GroupID)
{
    const float N = float(mapResolution);
    
    uint z = groupID.x;
    uint x = groupThreadID.x;
    
    uint nj = (reversebits(x) >> (32u - log2MapResolution)) & (mapResolution - 1u);
    pingpong[0u][nj] = input[uint2(z, x)];
    
    GroupMemoryBarrierWithGroupSync();
    
    uint src = 0u;
    
    [unroll]
    for (uint s = 1u; s <= log2MapResolution; ++s)
    {
        uint m = 1u << s;
        uint mh = m >> 1u;
        
        uint k = (x * (mapResolution / m)) & (mapResolution - 1u);
        uint i = (x & ~(m - 1u));
        uint j = (x & (mh - 1u));
        
        float theta = (TWO_PI * float(k)) / N;
        
        float2 W_N_k = float2(cos(theta), sin(theta));
        
        float2 input1 = pingpong[src][i + j + mh];
        float2 input2 = pingpong[src][i + j];
        
        src = 1u - src;
        pingpong[src][x] = input2 + ComplexMul(W_N_k, input1);
        
        GroupMemoryBarrierWithGroupSync();
    }
    
    output[uint2(x, z)] = pingpong[src][x];
}