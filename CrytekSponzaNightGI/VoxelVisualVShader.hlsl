struct VertexOutput
{
    uint vertexID : VERTEXID;
    float4 pos : SV_Position;
};

VertexOutput main(uint vertexID : SV_VertexID)
{
    VertexOutput output;
    output.vertexID = vertexID;
    output.pos = float4(1.0, 0.0, 0.0, 1.0);
    return output;
}