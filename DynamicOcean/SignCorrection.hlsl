RWTexture2D<float2> displacementY : register(u0);
RWTexture2D<float2> displacementX : register(u1);
RWTexture2D<float2> displacementZ : register(u2);

[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    float sign = ((((DTid.x + DTid.y) & 1u) == 1) ? -1.0 : 1.0);
    float2 v0 = displacementY[DTid.xy] * sign;
    float2 v1 = displacementX[DTid.xy] * sign;
    float2 v2 = displacementZ[DTid.xy] * sign;
    displacementY[DTid.xy] = v0;
    displacementX[DTid.xy] = -1.0 * v1;
    displacementZ[DTid.xy] = -1.0 * v2;
}