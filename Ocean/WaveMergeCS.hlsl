Texture2D<float2> displacementY : register(t0);
Texture2D<float2> displacementX : register(t1);
Texture2D<float2> displacementZ : register(t2);

RWTexture2D<float4> displacementXYZ : register(u0);

static const float lambda = -1.0;

[numthreads(32, 32, 1)]
void main(uint2 DTid : SV_DispatchThreadID)
{
    float v0 = displacementY[DTid].x;
    float v1 = lambda * displacementX[DTid].x;
    float v2 = lambda * displacementZ[DTid].x;
    
    displacementXYZ[DTid] = float4(v1, v0, v2, 1.0);
}