#define TWO_PI 6.283185307179586476925286766559

Texture2D<float2> input : register(t0);
RWTexture2D<float2> output : register(u0);

groupshared float2 pingpong[2][1024];

float2 ComplexMul(float2 z, float2 w)
{
    return float2(z.x * w.x - z.y * w.y, z.y * w.x + z.x * w.y);
}

[numthreads(1024, 1, 1)]
void main(int3 groupThreadID : SV_GroupThreadID, int3 groupID : SV_GroupID)
{
    const float N = 1024.0;
    
    int z = groupID.x;
    int x = groupThreadID.x;
    
    int nj = (reversebits(x) >> (32 - 10)) & (1024 - 1);
    pingpong[0][nj] = input[int2(z, x)];
    
    GroupMemoryBarrierWithGroupSync();
    
    int src = 0;
    
    [unroll]
    for (int s = 1; s <= 10; ++s)
    {
        int m = 1 << s;
        int mh = m >> 1;
        
        int k = (x * (1024 / m)) & (1024 - 1);
        int i = (x & ~(m - 1));
        int j = (x & (mh - 1));
        
        float theta = (TWO_PI * float(k)) / N;
        float2 W_N_k = float2(cos(theta), sin(theta));
        
        float2 input1 = pingpong[src][i + j + mh];
        float2 input2 = pingpong[src][i + j];
        
        src = 1 - src;
        pingpong[src][x] = input2 + ComplexMul(W_N_k, input1);
        
        GroupMemoryBarrierWithGroupSync();
    }
    
    output[int2(x, z)] = pingpong[src][x];
}