RWTexture2D<float2> output : register(u0);

static const float signs[2] = { 1.0, -1.0 };

[numthreads(32, 32, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    float sign = signs[(DTid.x + DTid.y) & 1];
    float2 value = output[DTid.xy] * sign;
    output[DTid.xy] = value;
}