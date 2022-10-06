#define M_PI 3.1415926535897932384626433832795

cbuffer OceanParam : register(b1)
{
    uint mapResolution;
    float mapLength;
    float2 wind;
    float amplitude;
    float gravity;
    float2 v0;
};

RWTexture2D<float2> tildeh0k : register(u0);
RWTexture2D<float2> tildeh0mk : register(u1);

Texture2D noiseTexture : register(t0);

float getPhillip(float2 k)
{
    float len = length(k);
    
    if (len < 0.000001)
        return 0.0;
        
    float len2 = len * len;
    float len4 = len2 * len2;
        
    float kDotw = dot(normalize(k), normalize(wind));
        
    float kDotw2 = kDotw * kDotw * kDotw * kDotw * kDotw * kDotw;
        
    float L = length(wind) * length(wind) / gravity;
        
    float L2 = L * L;
        
    float damping = 0.001;
        
    float l2 = L2 * damping * damping;
        
    return amplitude * exp(-1.0f / (len2 * L2)) / len4 * kDotw2 * exp(-len2 * l2);
}

float4 gaussRND(uint3 Coor)
{
    float4 noise = clamp(noiseTexture[Coor.xy], 0.001, 1);
    
    float u0 = 2.0 * M_PI * noise.r;
    float v0 = sqrt(-2.0 * log(noise.g));
    float u1 = 2.0 * M_PI * noise.b;
    float v1 = sqrt(-2.0 * log(noise.a));
    
    return float4(v0 * cos(u0), v0 * sin(u0), v1 * cos(u1), v1 * sin(u1));
}

[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    float2 k = float2(M_PI * (2.0 * float(DTid.x) - float(mapResolution)) / float(mapLength), M_PI * (2.0 * float(DTid.y) - float(mapResolution)) / float(mapLength));
    
    float ph1 = getPhillip(k);
    float ph2 = getPhillip(-k);
    float4 rnd = gaussRND(DTid);
    
    tildeh0k[DTid.xy] = float2(rnd.xy * sqrt(ph1 / 2.0));
    tildeh0mk[DTid.xy] = float2(rnd.zw * sqrt(ph2 / 2.0));
}