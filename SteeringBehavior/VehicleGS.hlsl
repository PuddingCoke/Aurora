struct GSInput
{
    float4 position : SV_Position;
    float2 velocity : VELOCITY;
};

struct GSOutput
{
    float2 texCoord : TEXCOORD;
    float4 position : SV_POSITION;
};

cbuffer ProjMatrix : register(b0)
{
    matrix proj;
}

cbuffer ViewMatrix : register(b1)
{
    matrix view;
    float4 viewPos;
    matrix prevViewProj;
    matrix viewProj;
    matrix normalMatrix;
};

[maxvertexcount(6)]
void main(
	point GSInput input[1],
	inout TriangleStream<GSOutput> outputs
)
{
    float2 right = normalize(input[0].velocity) * 8.0;
    
    float2 up = normalize(float2(-right.y, right.x)) * 5.0;
    
    GSOutput output;
    
    {
        output.position = mul(input[0].position + float4(-right - up, 0.0, 0.0), proj);
        output.texCoord = float2(0.0, 1.0);
        outputs.Append(output);
        
        output.position = mul(input[0].position + float4(-right + up, 0.0, 0.0), proj);
        output.texCoord = float2(0.0, 0.0);
        outputs.Append(output);
        
        output.position = mul(input[0].position + float4(right + up, 0.0, 0.0), proj);
        output.texCoord = float2(1.0, 0.0);
        outputs.Append(output);
    }
    
    {
        
        output.position = mul(input[0].position + float4(right + up, 0.0, 0.0), proj);
        output.texCoord = float2(1.0, 0.0);
        outputs.Append(output);
     
        output.position = mul(input[0].position + float4(-right - up, 0.0, 0.0), proj);
        output.texCoord = float2(0.0, 1.0);
        outputs.Append(output);
        
        output.position = mul(input[0].position + float4(right - up, 0.0, 0.0), proj);
        output.texCoord = float2(1.0, 1.0);
        outputs.Append(output);
    }
}