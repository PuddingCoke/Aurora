struct VertexOutput
{
    float2 vUv : TEXCOORD0;
    float2 vL : TEXCOORD1;
    float2 vR : TEXCOORD2;
    float2 vT : TEXCOORD3;
    float2 vB : TEXCOORD4;
    float4 position : SV_Position;
};

static const float2 positions[] =
{
    { -1.0, 1.0 },
    { 3.0, 1.0 },
    { -1.0, -3.0 }
};

static const float2 texCoords[] =
{
    { 0.0, 0.0 },
    { 2.0, 0.0 },
    { 0.0, 2.0 }
};

cbuffer SimulationConst : register(b1)
{
    float2 velocityTexelSize;
    float2 screenTexelSize;
    float2 sunraysTexelSizeX;
    float2 sunraysTexelSizeY;
    float velocity_dissipation;
    float density_dissipation;
    float value;
    float aspectRatio;
    float curl;
    float radius;
    float weight;
    float v0;
}

cbuffer SimulationDynamic : register(b2)
{
    float3 color0;
    float padding0;
    float3 color1;
    float padding1;
    float2 point0;
    float2 padding2;
}

VertexOutput main(in uint vertexID : SV_VertexID)
{
    VertexOutput output;
    output.vUv = texCoords[vertexID];
    output.vL = output.vUv - float2(screenTexelSize.x, 0.0);
    output.vR = output.vUv + float2(screenTexelSize.x, 0.0);
    output.vT = output.vUv + float2(0.0, screenTexelSize.y);
    output.vB = output.vUv - float2(0.0, screenTexelSize.y);
    output.position = float4(positions[vertexID], 0.0, 1.0);
    return output;
}