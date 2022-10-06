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

float2 complexMul(float2 a, float2 b)
{
    return float2(a.x * b.x - a.y * b.y, a.y * b.x + a.x * b.y);
}

Texture2D<float2> input : register(t0);
RWTexture2D<float2> output : register(u0);

groupshared float2 pingpong[2][1024];

[numthreads(1024, 1, 1)]
void main(uint3 groupThreadID : SV_GroupThreadID, uint3 groupID : SV_GroupID)
{
    const float N = float(mapResolution);
    
    int z = groupID.x;
    int x = groupThreadID.x;
    
    int nj = (reversebits(x) >> (32 - int(log2MapResolution))) & (int(mapResolution) - 1);
    pingpong[0][nj] = input[int2(z, x)];
    
    GroupMemoryBarrierWithGroupSync();
    
    int src = 0;
    
    [unroll]
    for (int s = 1; s <= int(log2MapResolution); ++s)
    {
        int m = 1u << s;
        int mh = m >> 1u;
        
        int k = (x * (int(mapResolution) / m)) & (int(mapResolution) - 1);
        int i = (x & ~(m - 1));
        int j = (x & (mh - 1));
        
        float theta = (TWO_PI * float(k)) / N;
        
        float2 W_N_k = float2(cos(theta), sin(theta));
        
        float2 input1 = pingpong[src][i + j + mh];
        float2 input2 = pingpong[src][i + j];
        
        src = 1 - src;
        pingpong[src][x] = input2 + complexMul(W_N_k, input1);
        
        GroupMemoryBarrierWithGroupSync();
    }
    
    output[int2(x, z)] = pingpong[src][x];
}