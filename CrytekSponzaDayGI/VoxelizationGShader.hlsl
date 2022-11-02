struct GeometryInput
{
    float3 pos : POSITION0;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT0;
    float3 bitangent : TANGENT1;
    float4 svPosition : SV_Position;
};

struct GeometryOutput
{
    float3 pos : POSITION0;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT0;
    float3 bitangent : TANGENT1;
    float4 svPosition : SV_Position;
};

[maxvertexcount(3)]
void main(
	triangle GeometryInput input[3],
	inout TriangleStream<GeometryOutput> output
)
{
    float3 p1 = input[0].svPosition.xyz;
    float3 p2 = input[1].svPosition.xyz;
    float3 p3 = input[2].svPosition.xyz;
    
    float3 projs = abs(cross(p2 - p1, p3 - p1));
    
    GeometryOutput gOutput;
    
    [unroll]
    for (uint i = 0; i < 3;i++)
    {
        gOutput.pos = input[i].pos;
        gOutput.uv = input[i].uv;
        gOutput.normal = input[i].normal;
        gOutput.tangent = input[i].tangent;
        gOutput.bitangent = input[i].bitangent;
        
        float3 p = input[i].svPosition.xyz;
        
        [flatten]
        if(projs.x>projs.y&&projs.x>projs.z)
        {
            gOutput.svPosition = float4(p.yz, 0.0, 1.0);
        }
        else if(projs.y>projs.z)
        {
            gOutput.svPosition = float4(p.xz, 0.0, 1.0);
        }
        else
        {
            gOutput.svPosition = float4(p.xy, 0.0, 1.0);
        }
        
        output.Append(gOutput);
    }
}