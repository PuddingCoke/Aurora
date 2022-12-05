#define PI 3.141592653589793

struct Sample
{
    float4 direction;
    float Ylm[36];
};

//NUMCOEFF x 1 r11g11b10
RWTexture2D<float3> irradianceCoeff : register(u0);

TextureCube envCube : register(t0);

StructuredBuffer<Sample> samples : register(t1);

SamplerState linearSampler : register(s0);

[numthreads(36, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    float3 result = float3(0.0, 0.0, 0.0);
    
    uint numSamples;
    uint stride;
    
    samples.GetDimensions(numSamples, stride);
    
    for (uint i = 0; i < numSamples; i++)
    {
        result += envCube.SampleLevel(linearSampler, samples[i].direction.xyz, 0.0).rgb * samples[i].Ylm[DTid.x];
    }
    
    result *= 4.0 * PI / float(numSamples);
    
    irradianceCoeff[DTid.xy] = result;
}