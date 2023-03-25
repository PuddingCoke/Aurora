Texture2D<uint> preState : register(t0);
RWTexture2D<uint> curState : register(u0);

cbuffer GameParam : register(b1)
{
    uint2 mapSize;
    float2 padding;
}

uint CountNeighbor(in uint2 coor)
{
    uint num = 0;
    num += uint(bool(preState[coor + uint2(-1, -1)]));
    num += uint(bool(preState[coor + uint2(0, -1)]));
    num += uint(bool(preState[coor + uint2(1, -1)]));
    num += uint(bool(preState[coor + uint2(-1, 0)]));
    num += uint(bool(preState[coor + uint2(1, 0)]));
    num += uint(bool(preState[coor + uint2(-1, 1)]));
    num += uint(bool(preState[coor + uint2(0, 1)]));
    num += uint(bool(preState[coor + uint2(1, 1)]));
    return num;
}

[numthreads(32, 18, 1)]
void main(uint2 DTid : SV_DispatchThreadID)
{
    if (preState[DTid])
    {
        if (DTid.x == 0 || DTid.x == mapSize.x - 1 || DTid.y == 0 || DTid.y == mapSize.y - 1)
        {
            curState[DTid] = 0;
            return;
        }
        
        const uint numNeighbor = CountNeighbor(DTid);
        
        if (numNeighbor == 2 || numNeighbor == 3)
        {
            curState[DTid] = 255;
        }
        else
        {
            curState[DTid] = 0;
        }
    }
    else
    {
        const uint numNeighbor = CountNeighbor(DTid);
        
        if (numNeighbor == 3)
        {
            curState[DTid] = 255;
        }
        else
        {
            curState[DTid] = 0;
        }
    }
}