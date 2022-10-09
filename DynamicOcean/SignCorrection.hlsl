Texture2D<float2> displacementY : register(t0);
Texture2D<float2> displacementX : register(t1);
Texture2D<float2> displacementZ : register(t2);
Texture2D<float2> slopeX : register(t3);
Texture2D<float2> slopeZ : register(t4);

RWTexture2D<float4> displacementXYZ : register(u0);
RWTexture2D<float4> normalTexture : register(u1);

static const float lambda = -1.0;

[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    float sign = ((((DTid.x + DTid.y) & 1u) == 1) ? -1.0 : 1.0);
    
    float v0 = displacementY[DTid.xy].x * sign;
    float v1 = displacementX[DTid.xy].x * sign;
    float v2 = displacementZ[DTid.xy].x * sign;
    
    float v3 = slopeX[DTid.xy].x * sign;
    float v4 = slopeZ[DTid.xy].x * sign;
    
    displacementXYZ[DTid.xy] = float4(lambda * v1, v0, lambda * v2, 1.0);
    normalTexture[DTid.xy] = float4(normalize(float3(-v3, 1.0, -v4)), 1.0);
}