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

#define EDGENUM 16
static const float cosdTheta = cos(-3.1415926535 / float(EDGENUM));
static const float sindTheta = sin(-3.1415926535 / float(EDGENUM));

float2 rotTheta(float2 v)
{
    return float2(v.x * cosdTheta - v.y * sindTheta, v.x * sindTheta + v.y * cosdTheta);
}

float2 rotNTheta(float2 v)
{
    return float2(v.x * cosdTheta + v.y * sindTheta, -v.x * sindTheta + v.y * cosdTheta);
}

[maxvertexcount(2 * EDGENUM + 2)]
void main(
	line GeometryInput input[2],
	inout TriangleStream<GeometryOutput> outputs
)
{
    const float2 dir = input[0].width * normalize(input[1].pos.xy - input[0].pos.xy);
    
    GeometryOutput output;
    output.color = input[0].color;
    
    float2 v0 = -dir;
    float2 v1 = -dir;
    
    output.pos = mul(input[0].pos - float4(dir, 0.0, 0.0), proj);
    outputs.Append(output);
    
    [unroll]
    for (uint i = 0; i < EDGENUM / 2; i++)
    {
        v0 = rotTheta(v0);
        v1 = rotNTheta(v1);
        
        output.pos = mul(input[0].pos + float4(v0, 0.0, 0.0), proj);
        outputs.Append(output);
        
        output.pos = mul(input[0].pos + float4(v1, 0.0, 0.0), proj);
        outputs.Append(output);
    }
    
    output.pos = mul(input[1].pos + float4(v0, 0.0, 0.0), proj);
    outputs.Append(output);
        
    output.pos = mul(input[1].pos + float4(v1, 0.0, 0.0), proj);
    outputs.Append(output);
    
    [unroll]
    for (i = 0; i < EDGENUM / 2 - 1; i++)
    {
        v0 = rotTheta(v0);
        v1 = rotNTheta(v1);
        
        output.pos = mul(input[1].pos + float4(v0, 0.0, 0.0), proj);
        outputs.Append(output);
        
        output.pos = mul(input[1].pos + float4(v1, 0.0, 0.0), proj);
        outputs.Append(output);
    }
    
    output.pos = mul(input[1].pos + float4(dir, 0.0, 0.0), proj);
    outputs.Append(output);
}