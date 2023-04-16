Texture2D<float> depthRead : register(t0);
RWTexture2D<float> depthWrite : register(u0);

[numthreads(16, 9, 1)]
void main(uint2 DTid : SV_DispatchThreadID)
{
    depthWrite[DTid] = depthRead[DTid];
}