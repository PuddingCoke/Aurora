struct GeometryInput
{
    float4 pos : SV_Position;
    float width : POSITION;
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

#define TRIANGLENUM 8
static const float cosdTheta = cos(-3.1415926535 / float(TRIANGLENUM));
static const float sindTheta = sin(-3.1415926535 / float(TRIANGLENUM));

float2 rotTheta(float2 v)
{
    return float2(v.x * cosdTheta - v.y * sindTheta, v.x * sindTheta + v.y * cosdTheta);
}

[maxvertexcount((TRIANGLENUM - 1) * 6)]
void main(
	line GeometryInput input[2],
	inout TriangleStream<GeometryOutput> outputs
)
{
    const float2 dir = normalize(input[1].pos.xy - input[0].pos.xy);
    const float2 v = input[0].width * float2(-dir.y, dir.x);
    
    GeometryOutput output;
    output.color = input[0].color;
    
    float2 v0 = v;
    float2 v1 = rotTheta(v0);
    
    [unroll]
    for (uint i = 0; i < TRIANGLENUM - 1; i++)
    {
        output.pos = mul(input[0].pos - float4(v, 0.0, 0.0), proj);
        outputs.Append(output);
        
        output.pos = mul(input[1].pos + float4(v0, 0.0, 0.0), proj);
        outputs.Append(output);
        
        output.pos = mul(input[1].pos + float4(v1, 0.0, 0.0), proj);
        outputs.Append(output);
        
        v0 = v1;
        v1 = rotTheta(v1);
    }
    
    v0 = v1;
    v1 = rotTheta(v1);
    
    [unroll]
    for (i = 0; i < TRIANGLENUM - 1; i++)
    {
        output.pos = mul(input[1].pos + float4(v, 0.0, 0.0), proj);
        outputs.Append(output);
        
        output.pos = mul(input[0].pos + float4(v1, 0.0, 0.0), proj);
        outputs.Append(output);
        
        output.pos = mul(input[0].pos + float4(v0, 0.0, 0.0), proj);
        outputs.Append(output);
        
        v0 = v1;
        v1 = rotTheta(v1);
    }
}