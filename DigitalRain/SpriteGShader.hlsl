struct GSInput
{
    float4 position : SV_Position;
    float2 size : SIZE;
    float4 color : COLOR;
    float uvLeft : TEXCOORD0;
    float uvRight : TEXCOORD1;
    float uvBottom : TEXCOORD2;
    float uvTop : TEXCOORD3;
};

struct GSOutput
{
    float2 texCoord : TEXCOORD;
    float4 color : COLOR;
    float4 pos : SV_POSITION;
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
}

[maxvertexcount(6)]
void main(
	point GSInput input[1],
	inout TriangleStream<GSOutput> outputs
)
{
    GSOutput output;
    output.color = input[0].color;
    
    float3 cameraRight = float3(view[0][0], view[1][0], view[2][0]);
    float3 cameraUp = float3(view[0][1], view[1][1], view[2][1]);
    
    output.pos = mul(input[0].position + float4(cameraRight * -0.5 * input[0].size.x + cameraUp * -0.5 * input[0].size.y, 0.0), viewProj);
    output.texCoord = float2(input[0].uvLeft, input[0].uvBottom);
    outputs.Append(output);
    
    output.pos = mul(input[0].position + float4(cameraRight * -0.5 * input[0].size.x + cameraUp * 0.5 * input[0].size.y, 0.0), viewProj);
    output.texCoord = float2(input[0].uvLeft, input[0].uvTop);
    outputs.Append(output);
    
    output.pos = mul(input[0].position + float4(cameraRight * 0.5 * input[0].size.x + cameraUp * 0.5 * input[0].size.y, 0.0), viewProj);
    output.texCoord = float2(input[0].uvRight, input[0].uvTop);
    outputs.Append(output);
    
    output.pos = mul(input[0].position + float4(cameraRight * 0.5 * input[0].size.x + cameraUp * 0.5 * input[0].size.y, 0.0), viewProj);
    output.texCoord = float2(input[0].uvRight, input[0].uvTop);
    outputs.Append(output);
    
    output.pos = mul(input[0].position + float4(cameraRight * 0.5 * input[0].size.x + cameraUp * -0.5 * input[0].size.y, 0.0), viewProj);
    output.texCoord = float2(input[0].uvRight, input[0].uvBottom);
    outputs.Append(output);
    
    output.pos = mul(input[0].position + float4(cameraRight * -0.5 * input[0].size.x + cameraUp * -0.5 * input[0].size.y, 0.0), viewProj);
    output.texCoord = float2(input[0].uvLeft, input[0].uvBottom);
    outputs.Append(output);
}