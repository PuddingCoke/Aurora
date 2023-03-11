#define MAXITERATION 300

cbuffer SimulationParam : register(b1)
{
    float2 pos;
    float size;
    float aspectRatio;
}

//x+yi
double2 ComplexSquare(double2 z)
{
    return double2(z.x * z.x - z.y * z.y, double(2.0) * z.x * z.y);
}

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    double2 planePos = texCoord - 0.5;
    planePos.x *= aspectRatio;
    
    double2 curPos = pos + planePos * size;
    
    const double2 originPos = curPos;
    
    uint i = 0;
    
    [loop]
    for (; i < MAXITERATION; i++)
    {
        curPos = ComplexSquare(curPos) + originPos;
        if (curPos.x * curPos.x + curPos.y * curPos.y > 16.0)
        {
            break;
        }
    }
    
    return float4(float(i) / float(MAXITERATION) * float3(1.0, 1.0, 1.0), 1.0);
}