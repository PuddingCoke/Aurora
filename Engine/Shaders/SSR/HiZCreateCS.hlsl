Texture2D<float> depthRead : register(t0);
RWTexture2D<float> depthWrite : register(u0);

[numthreads(16, 9, 1)]
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
    
    uint2 resolution;
    
    depthRead.GetDimensions(resolution.x, resolution.y);
    
    bool extraColumn = ((resolution.x & 1) != 0);
    
    bool extraRow = ((resolution.y & 1) != 0);
    
    if(extraColumn)
    {
        float2 extraDepth = float2(depthRead[readCoord + uint2(2, 0)], depthRead[readCoord + uint2(2, 1)]);
        
        if(extraRow)
        {
            float cornerDepth = depthRead[readCoord + uint2(2, 2)];
            
            minDepth = min(minDepth, cornerDepth);
        }
        
        minDepth = min(minDepth, min(extraDepth.x, extraDepth.y));
    }
    
    if(extraRow)
    {
        float2 extraDepth = float2(depthRead[readCoord + uint2(0, 2)], depthRead[readCoord + uint2(1, 2)]);
        
        minDepth = min(minDepth, min(extraDepth.x, extraDepth.y));
    }
    
    depthWrite[DTid] = minDepth;
}