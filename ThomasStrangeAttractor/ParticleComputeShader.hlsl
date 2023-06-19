RWBuffer<float4> positions : register(u0);

cbuffer DeltaTimes : register(b0)
{
    float deltaTime;
    float v1;
    float v2;
    float v3;
};

cbuffer SimulationParam : register(b1)
{
    float factor;
    float3 padding;
}

[numthreads(1000, 1, 1)]
void main(in uint3 DTid : SV_DispatchThreadID)
{
    float4 pos = positions[DTid.x];
    
    [unroll]
    for (int i = 0; i < 16; i++)
    {
        const float dx = (sin(pos.y) - factor * pos.x) * deltaTime * 0.5;
        const float dy = (sin(pos.z) - factor * pos.y) * deltaTime * 0.5;
        const float dz = (sin(pos.x) - factor * pos.z) * deltaTime * 0.5;

        pos += float4(dx, dy, dz, 0.0);
    }
    
    positions[DTid.x] = pos;
}