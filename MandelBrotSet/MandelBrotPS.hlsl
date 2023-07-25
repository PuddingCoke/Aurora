#define MAXITERATION 256

cbuffer SimulationParam : register(b1)
{
    double2 pos;
    double size;
    double aspectRatio;
}

//x+yi
inline double2 ComplexSquare(double2 z)
{
    return double2(z.x * z.x - z.y * z.y, double(2.0) * z.x * z.y);
}

#define PI 3.1415926535

float3 HSVtoRGB(float h, float s, float v)
{
    float c = v * s; // chroma     
    float x = c * (1.0 - abs(fmod(h / 60.0, 2.0) - 1.0)); // intermediate value     
    float m = v - c; // offset     
    float3 rgb; // output color     
    if (h < 60.0)
        rgb = float3(c, x, 0.0);
    else if (h < 120.0)
        rgb = float3(x, c, 0.0);
    else if (h < 180.0)
        rgb = float3(0.0, c, x);
    else if (h < 240.0)
        rgb = float3(0.0, x, c);
    else if (h < 300.0)
        rgb = float3(x, 0.0, c);
    else
        rgb = float3(c, 0.0, x);
    return rgb + m; // add offset 
}

float3 getColor1(float d)
{
    float hue = d * 360.0 + 200.0; // map d to hue     
    float saturation = 1.0 - d; // set saturation to 1     
    float value = 1.0 - d; // set value to 1
    
    return HSVtoRGB(hue, saturation, value); // convert HSV to RGB 
}

float3 getColor2(float d)
{
    float hue = d * 360.0+100.0; // map d to hue     
    float saturation = 1.0 - d; // set saturation to 1     
    float value = 1.0 - d; // set value to 1
    
    return HSVtoRGB(hue, saturation, value); // convert HSV to RGB 
}

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    double2 planePos = double2(texCoord - 0.5);
    
    planePos.x *= aspectRatio;
    
    const double2 curPos = pos + planePos * size;
    
    double2 z = double2(0.0, 0.0);
    
    double2 dz = double2(1.0, 0.0);
    
    double dist = 1e20;
    
    [loop]
    for (uint i = 0; i < MAXITERATION; i++)
    {
        dz = 2.0 * double2(z.x * dz.x - z.y * dz.y, z.x * dz.y + z.y * dz.x) + double2(1.0, 0.0);
        
        z = ComplexSquare(z) + curPos;
        
        if ((z.x * z.x + z.y * z.y) > double(64.0))
        {
            break;
        }
        
        dist = min(dist, length(z));
    }
    
    float d = sqrt(dot(z, z) / dot(dz, dz)) * log(dot(z, z));
    
    d = sqrt(saturate(8.0 / size * d));
    
    dist = pow(saturate(dist * 0.4), 0.5);
    
    return float4(lerp(getColor1(d), getColor2(dist), 1.0 - dist), 1.0);
}