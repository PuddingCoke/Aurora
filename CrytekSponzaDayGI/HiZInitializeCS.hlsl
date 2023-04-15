Texture2D<float> depthRead : register(t0);
RWTexture2D<float> depthWrite : register(u0);

[numthreads(8, 8, 1)]
void main(uint2 DTid : SV_DispatchThreadID)
{
    uint2 readCoord = DTid << 1;
    
    float4 depths = float4
    (
    depthRead[readCoord + uint2(0, 0)],
    depthRead[readCoord + uint2(1, 0)],
    depthRead[readCoord + uint2(0, 1)],
    depthRead[readCoord + uint2(1, 1)]
    );
    
    float minDepth = min(min(depths.x, depths.y), min(depths.z, depths.w));
    
    depthWrite[DTid] = minDepth;
}