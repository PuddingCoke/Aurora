Texture2D<float> curState : register(t0);
RWTexture2D<float4> outputTexture : register(u0);

[numthreads(16, 9, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    outputTexture[DTid.xy] = float4(curState[DTid.xy].rrr, 1.0);
}