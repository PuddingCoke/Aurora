RWTexture2D<float> outTex : register(u0);

[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    float sign = ((((DTid.x + DTid.y) & 1u) == 1) ? -1.0 : 1.0);
    float result = outTex[DTid.xy] * sign;
    outTex[DTid.xy] = result;
}