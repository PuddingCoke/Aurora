RWBuffer<float4> positions:register(u1);

cbuffer DeltaTimes : register(b0)
{
    float deltaTime;
    float v1;
    float v2;
    float v3;
};

static const float factor = 0.18;

[numthreads(1000, 1, 1)]
void main(in uint3 DTid : SV_DispatchThreadID )
{
    float4 pos = positions[DTid.x];
    
    [unroll]
    for (int i = 0; i < 6;i++)
    {
        const float dx = (sin(pos.y) - factor * pos.x) * deltaTime;
        const float dy = (sin(pos.z) - factor * pos.y) * deltaTime;
        const float dz = (sin(pos.x) - factor * pos.z) * deltaTime;

        pos.x += dx;
        pos.y += dy;
        pos.z += dz;
    }
    
    positions[DTid.x] = pos;
}