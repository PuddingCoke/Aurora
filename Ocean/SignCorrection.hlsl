Texture2D<float2> displacementY : register(t0);
Texture2D<float2> displacementX : register(t1);
Texture2D<float2> displacementZ : register(t2);

RWTexture2D<float4> displacementXYZ : register(u0);

[numthreads(32, 32, 1)]
void main(uint2 DTid : SV_DispatchThreadID)
{
    float sign = ((((DTid.x + DTid.y) & 1u) == 1) ? -1.0 : 1.0);
    
    float v0 = displacementY[DTid].x * sign;
    float v1 = -displacementX[DTid].x * sign;
    float v2 = -displacementZ[DTid].x * sign;
    
    displacementXYZ[DTid] = float4(v1, v0, v2, 1.0);
}