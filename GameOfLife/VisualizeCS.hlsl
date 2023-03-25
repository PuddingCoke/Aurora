Texture2D<uint> curState : register(t0);
RWTexture2D<float4> outputTexture : register(u0);

cbuffer GameParam : register(b1)
{
    uint2 mapSize;
    float2 padding;
}

static const int area = 9;

float Density(in uint2 coor)
{
    uint num = 0;
    
    [unroll]
    for (int x = -area; x <= area; x++)
    {
        [unroll]
        for (int y = -area; y <= area; y++)
        {
            num += uint(bool(curState[int2(coor) + int2(x, y)]));
        }
    }
    
    return float(num) / float((area + 1) * (area + 1));
}

[numthreads(32, 18, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    if (curState[DTid.xy])
    {
        float density = Density(DTid.xy);
        
        outputTexture[DTid.xy] = float4(0.0, density, density, 1.0);
    }
    else
    {
        outputTexture[DTid.xy] = float4(1.0, 1.0, 1.0, 1.0);
    }
}