struct GSOutput
{
    float4 color : COLOR;
    float4 svPosition : SV_Position;
};

cbuffer ViewMatrix : register(b1)
{
    matrix view;
    float4 viewPos;
    matrix prevViewProj;
    matrix viewProj;
    matrix normalMatrix;
};

cbuffer DeltaTime : register(b2)
{
    float dt;
}

cbuffer SimulationParam : register(b3)
{
    float factor;
    float3 padding;
}

void GetNextPosition(inout float4 pos)
{
    [unroll]
    for (int i = 0; i < 5; i++)
    {
        const float dx = (sin(pos.y) - factor * pos.x) * dt;
        const float dy = (sin(pos.z) - factor * pos.y) * dt;
        const float dz = (sin(pos.x) - factor * pos.z) * dt;

        pos += float4(dx, dy, dz, 0.0);
    }
}

[maxvertexcount(6)]
void main(
	point GSOutput input[1],
	inout LineStream<GSOutput> output
)
{
    GSOutput o;
    o.color = input[0].color;
    float4 position = input[0].svPosition;
    [unroll]
    for (uint i = 0; i < 6; i++)
    {
        o.svPosition = mul(position, viewProj);
        output.Append(o);
        GetNextPosition(position);
    }
}