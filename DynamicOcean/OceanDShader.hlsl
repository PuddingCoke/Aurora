struct DS_OUTPUT
{
    float3 position : POSITION;
    float2 texCoord : TEXCOORD;
	float4 vPosition  : SV_POSITION;
};

struct HS_CONTROL_POINT_OUTPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
};

struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[4]			: SV_TessFactor;
	float InsideTessFactor[2]			: SV_InsideTessFactor;
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

float3 getPosition(float3 v0,float3 v1,float3 v2,float3 v3,float2 uv)
{
    float3 bottom = lerp(v0, v1, uv.x);
    float3 top = lerp(v3, v2, uv.x);
    float3 p = lerp(bottom, top, uv.y);
    return p;
}

float2 getUV(float2 v0, float2 v1, float2 v2, float2 v3, float2 uv)
{
    float2 bottom = lerp(v0, v1, uv.x);
    float2 top = lerp(v3, v2, uv.x);
    float2 p = lerp(bottom, top, uv.y);
    return p;
}

Texture2D displacementXYZ : register(t0);

SamplerState linearSampler : register(s0);

[domain("quad")]
DS_OUTPUT main(
	HS_CONSTANT_DATA_OUTPUT input,
	float2 domain : SV_DomainLocation,
	const OutputPatch<HS_CONTROL_POINT_OUTPUT, 4> patch)
{
	DS_OUTPUT Output;
    float3 position = getPosition(patch[0].position, patch[1].position, patch[2].position, patch[3].position, domain);
    float2 uv = getUV(patch[0].uv, patch[1].uv, patch[2].uv, patch[3].uv, domain);
    Output.position = position;
    Output.texCoord = uv;
    position += displacementXYZ.SampleLevel(linearSampler, uv, 0.0).xyz;
    Output.vPosition = mul(mul(float4(position, 1.0), view), proj);
	return Output;
}
