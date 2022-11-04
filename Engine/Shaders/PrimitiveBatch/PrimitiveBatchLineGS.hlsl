struct GeometryInput
{
	float4 pos : SV_Position;
	float4 color : COLOR;
};

struct GeometryOutput
{
    float4 color : COLOR;
    float4 pos : SV_Position;
};

cbuffer Matrix2D : register(b0)
{
    matrix proj;
};

cbuffer LineBuffer : register(b2)
{
	float lineWidth;
	float v1;
	float v2;
	float v3;
};

[maxvertexcount(6)]
void main(
	line GeometryInput input[2],
	inout TriangleStream<GeometryOutput> outputs
)
{
    const float2 dir = input[1].pos.xy - input[0].pos.xy;
    const float2 dirN = normalize(dir);
    const float2 v = lineWidth / 2.0 * float2(-dirN.y, dirN.x);
    
    GeometryOutput rect[6];
    
    rect[0].color = input[0].color;
    rect[1].color = input[0].color;
    rect[2].color = input[0].color;
    rect[3].color = input[0].color;
    rect[4].color = input[0].color;
    rect[5].color = input[0].color;
    
    rect[0].pos = mul(input[0].pos + float4(v, 0.0, 0.0), proj);
    rect[1].pos = mul(input[1].pos + float4(v, 0.0, 0.0), proj);
    rect[2].pos = mul(input[0].pos - float4(v, 0.0, 0.0), proj);
    rect[3].pos = mul(input[1].pos + float4(v, 0.0, 0.0), proj);
    rect[4].pos = mul(input[1].pos - float4(v, 0.0, 0.0), proj);
    rect[5].pos = mul(input[0].pos - float4(v, 0.0, 0.0), proj);
    
    outputs.Append(rect[0]);
    outputs.Append(rect[1]);
    outputs.Append(rect[2]);
    outputs.Append(rect[3]);
    outputs.Append(rect[4]);
    outputs.Append(rect[5]);
}