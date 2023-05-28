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
RWTexture2D<float4> waveData : register(u1);

Texture2D gaussTexture : register(t0);

float Dispersion(float2 k)
{
    float w_0 = M_PI / 100.0;
    return floor(sqrt(gravity * length(k)) / w_0) * w_0;
}

float getPhillip(float2 k)
{
    float len = length(k);
        
    float len2 = len * len;
    float len4 = len2 * len2;
        
    float kDotw = dot(normalize(k), normalize(wind));
    
    float kDotw6 = kDotw * kDotw * kDotw * kDotw * kDotw * kDotw;
        
    float L = length(wind) * length(wind) / gravity;
        
    float L2 = L * L;
        
    float damping = 0.001;
        
    float l2 = L2 * damping * damping;
    
    if (kDotw < 0.0)
    {
        return amplitude * exp(-1.0 / (len2 * L2)) / len4 * kDotw6 * exp(-len2 * l2) * 0.80;
    }
    
    return amplitude * exp(-1.0 / (len2 * L2)) / len4 * kDotw6 * exp(-len2 * l2);
}

float2 conj(float2 a)
{
    return float2(a.x, -a.y);
}

[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    float2 k = float2(M_PI * (2.0 * float(DTid.x) - float(mapResolution)) / float(mapLength), M_PI * (2.0 * float(DTid.y) - float(mapResolution)) / float(mapLength));
    
    float len = length(k);
    
    if (len >= 0.0001 && len <= 5.0)
    {
        tildeh0k[DTid.xy] = gaussTexture[DTid.xy].xy * sqrt(getPhillip(k) / 2.0);
        waveData[DTid.xy] = float4(k.x, 1.0 / len, k.y, Dispersion(k));
    }
    else
    {
        tildeh0k[DTid.xy] = float2(0.0, 0.0);
        waveData[DTid.xy] = float4(k.x, 1.0, k.y, 0.0);
    }
}