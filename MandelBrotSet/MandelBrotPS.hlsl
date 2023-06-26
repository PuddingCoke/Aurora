#define MAXITERATION 200

cbuffer SimulationParam : register(b1)
{
    float2 pos;
    float size;
    float aspectRatio;
}

//x+yi
float2 ComplexSquare(float2 z)
{
    return float2(z.x * z.x - z.y * z.y, 2.0 * z.x * z.y);
}

static const float4 colors[10] =
{
    float4(0.0, 0.0, 0.0, 1.0), // black
    float4(0.5, 0.0, 1.0, 1.0), // purple
    float4(0.0, 0.0, 1.0, 1.0), // blue
    float4(0.0, 1.0, 1.0, 1.0), // cyan
    float4(0.0, 1.0, 0.0, 1.0), // green
    float4(1.0, 1.0, 0.0, 1.0), // yellow
    float4(1.0, 0.5, 0.0, 1.0), // orange
    float4(1.0, 0.0, 0.0, 1.0), // red
    float4(1.0, 0.5, 1.0, 1.0), // pink
    float4(1.0, 1.0, 1.0, 1.0) // white
};

//new bing generated
float3 GetColor(float value)
{
    // Clamp the value to the range [0, 1]
    value = saturate(value);
    
    // Scale the value to the range [0, colors.length - 1]
    value *= 9.0;
    
    // Get the lower and upper indices of the colors to interpolate between
    uint lower = uint(floor(value));
    uint upper = uint(ceil(value));
    
    // Get the fraction to use for interpolation
    float fraction = frac(value);
    
    // Interpolate between the colors using a linear blend
    return lerp(colors[lower], colors[upper], fraction);
}

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    float2 planePos = texCoord - 0.5;
    
    planePos.x *= aspectRatio;
    
    float2 curPos = pos + planePos * size;
    
    const float2 originPos = curPos;
    
    uint i = 0;
    
    [loop]
    for (; i < MAXITERATION && (curPos.x * curPos.x + curPos.y * curPos.y) < 16.0; i++)
    {
        curPos = ComplexSquare(curPos) + originPos;
    }
    
    return float4(GetColor(float(i) / float(MAXITERATION)), 1.0);
}