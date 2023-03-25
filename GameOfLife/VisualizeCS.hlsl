Texture2D<uint> curState : register(t0);
RWTexture2D<float4> outputTexture : register(u0);

cbuffer DeltaTime : register(b0)
{
    float dt;
    float sTime;
    uint uintSeed;
    float floatSeed;
}

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
    
    return float(num) / float((area + 1) * (area + 1)) + 0.3;
}

[numthreads(32, 18, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    float2 texCoord = float2(DTid.xy + 0.5) / float2(mapSize);
    
    if (curState[DTid.xy])
    {
        float density = Density(DTid.xy);
        
        float3 color = 0.5 * sin(texCoord.xyx + float3(sTime, sTime + 2.0, sTime + 4.0)) + 0.5;
        
        color *= density;
        
        outputTexture[DTid.xy] = float4(color, 1.0);
    }
    else
    {
        outputTexture[DTid.xy] = float4(0.0, 0.0, 0.0, 1.0);
    }
}