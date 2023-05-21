struct VS_CONTROL_POINT_OUTPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
};

struct HS_CONTROL_POINT_OUTPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
};

struct HS_CONSTANT_DATA_OUTPUT
{
    float EdgeTessFactor[4] : SV_TessFactor;
    float InsideTessFactor[2] : SV_InsideTessFactor;
};

cbuffer ProjMatrix : register(b0)
{
    matrix proj;
}

cbuffer ViewMatrix : register(b1)
{
    matrix view;
    float4 viewPos;
}

HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(
	InputPatch<VS_CONTROL_POINT_OUTPUT, 4> ip,
	uint PatchID : SV_PrimitiveID)
{
    HS_CONSTANT_DATA_OUTPUT Output;
    
    float3 centerPos = float3(0.0, 0.0, 0.0);
    
    centerPos += ip[0].position;
    centerPos += ip[1].position;
    centerPos += ip[2].position;
    centerPos += ip[3].position;

    centerPos /= 4.0;
    
    float dist = distance(viewPos.xyz, centerPos);
    
    float tessFactor = 8.0 + distance(ip[0].position, ip[1].position) / dist;
    
    Output.EdgeTessFactor[0] = tessFactor;
    Output.EdgeTessFactor[1] = tessFactor;
    Output.EdgeTessFactor[2] = tessFactor;
    Output.EdgeTessFactor[3] = tessFactor;
    Output.InsideTessFactor[0] = (Output.EdgeTessFactor[0] + Output.EdgeTessFactor[1] + Output.EdgeTessFactor[2]) / 3.0;
    Output.InsideTessFactor[1] = (Output.EdgeTessFactor[2] + Output.EdgeTessFactor[3] + Output.EdgeTessFactor[0]) / 3.0;

    return Output;
}

[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("CalcHSPatchConstants")]
HS_CONTROL_POINT_OUTPUT main(
	InputPatch<VS_CONTROL_POINT_OUTPUT, 4> ip,
	uint i : SV_OutputControlPointID)
{
    HS_CONTROL_POINT_OUTPUT Output;
    Output.position = ip[i].position;
    Output.uv = ip[i].uv;
    return Output;
}
