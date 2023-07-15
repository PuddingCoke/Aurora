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
    //iteration number decide trail length
    [unroll]
    for (int i = 0; i < 30; i++)
    {
        const float dx = (sin(pos.y) - factor * pos.x) * dt;
        const float dy = (sin(pos.z) - factor * pos.y) * dt;
        const float dz = (sin(pos.x) - factor * pos.z) * dt;

        pos += float4(-dx, -dy, -dz, 0.0);
    }
}

[maxvertexcount(13)]
void main(
	point GSOutput input[1],
	inout TriangleStream<GSOutput> output
)
{
    GSOutput o;
    float4 position = input[0].svPosition;
    
    float3 cameraRight = float3(view[0][0], view[1][0], view[2][0]);
    float3 cameraUp = float3(view[0][1], view[1][1], view[2][1]);
    
    o.color = float4(0.0, 0.0, 0.0, 1.0);
    o.svPosition = mul(position, viewProj);
    output.Append(o);
    
    float3 normal = normalize(cross(cameraRight, cameraUp));
    
    [unroll]
    for (uint i = 0; i < 6; i++)
    {
        float4 prevPos = position;
        
        GetNextPosition(position);
        
        float3 vec = normalize(cross(normal, normalize(position.xyz - prevPos.xyz)));
        
        float factor = 1.0 - float(i + 1) / 6.0;
        
        float width = 0.005 * factor;
        
        o.color = float4(input[0].color.rgb * factor, 1.0);
        
        o.svPosition = mul(position + float4(width * vec, 0.0), viewProj);
        output.Append(o);
        
        o.svPosition = mul(position - float4(width * vec, 0.0), viewProj);
        output.Append(o);
    }
}