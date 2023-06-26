Texture2D<float> curState : register(t0);
RWTexture2D<float4> outputTexture : register(u0);

cbuffer DeltaTime : register(b0)
{
    float dt;
    float sTime;
    uint uintSeed;
    float floatSeed;
}

[numthreads(16, 9, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    outputTexture[DTid.xy] = float4(curState[DTid.xy].rrr * float3(DTid.xy / float2(1920.0, 1080.0), 1.0), 1.0);
}