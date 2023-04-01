struct VertexOutput
{
    float4 probePosition : SV_Position;
    uint probeIndex : PROBEINDEX;
};

cbuffer IrradianceVolumeParam : register(b2)
{
    float3 fieldStart;
    float probeSpacing;
    uint3 probeCount;
    float padding;
};

float3 ProbeGridPosToLoc(uint3 probeGridPos)
{
    return fieldStart + float3(probeGridPos) * probeSpacing;
}

uint3 ProbeIndexToGridPos(uint probeIndex)
{
    return uint3(
    (probeIndex % (probeCount.x * probeCount.z)) % probeCount.x,
    probeIndex / (probeCount.x * probeCount.z),
    (probeIndex % (probeCount.x * probeCount.z)) / probeCount.x
    );

}

VertexOutput main(uint vertexID : SV_VertexID)
{
    VertexOutput output;
    output.probePosition = float4(ProbeGridPosToLoc(ProbeIndexToGridPos(vertexID)), 1.0);
    output.probeIndex = vertexID;
    return output;
}