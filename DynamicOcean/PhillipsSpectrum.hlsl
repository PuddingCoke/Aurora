cbuffer OceanParam : register(b1)
{
    uint mapResolution;
    float mapLength;
    float2 wind;
    float phillipParam;
    float gravity;
    float2 v0;
};

RWTexture2D<float> phillipSpectrum : register(u0);

#define M_PI 3.1415926535897932384626433832795

[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    float2 k = float2(M_PI * (2.0 * float(DTid.x) - float(mapResolution)) / float(mapLength), M_PI * (2.0 * float(DTid.y) - float(mapResolution)) / float(mapLength));
    
    float len = length(k);
    
    if (len < 0.000001)
    {
        phillipSpectrum[DTid.xy] = float(0.0);
    }
    else
    {
        float len2 = len * len;
        float len4 = len2 * len2;
        
        float kDotw = dot(normalize(k), normalize(wind));
        
        float kDotw2 = kDotw * kDotw * kDotw * kDotw * kDotw * kDotw;
        
        float L = length(wind) * length(wind) / gravity;
        
        float L2 = L * L;
        
        float damping = 0.001;
        
        float l2 = L2 * damping * damping;
        
        phillipSpectrum[DTid.xy] = float(phillipParam * exp(-1.0f / (len2 * L2)) / len4 * kDotw2 * exp(-len2 * l2));
    }
}