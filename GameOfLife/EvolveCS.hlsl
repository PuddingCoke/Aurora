#define PI 3.1415926535897932384626433832

static const float ra = 12.0;
static const float rr = 3.0;
static const float b1 = 0.278;
static const float b2 = 0.365;
static const float d1 = 0.267;
static const float d2 = 0.445;
static const float alpha_n = 0.028;
static const float alpha_m = 0.147;

Texture2D<float> prevState : register(t0);
RWTexture2D<float> curState : register(u0);

SamplerState samplerState : register(s0);

float sigma1(float x, float a, float alpha)
{
    return 1.0 / (1.0 + exp(-(x - a) * 4.0 / alpha));
}

float sigma2(float x, float a, float b, float alpha)
{
    return sigma1(x, a, alpha) * (1.0 - sigma1(x, b, alpha));
}

float sigmam(float x, float y, float m, float alpha)
{
    return x * (1.0 - sigma1(m, 0.5, alpha)) + y * sigma1(m, 0.5, alpha);
}

float s(float n, float m)
{
    return sigma2(n, sigmam(b1, d1, m, alpha_m), sigmam(b2, d2, m, alpha_m), alpha_n);
}

float clampFactor(float x, float a, float ea)
{
    return saturate((x - a) / ea + 0.5);
}

[numthreads(16, 9, 1)]
void main(int2 DTid : SV_DispatchThreadID)
{
    float rb = ra / rr;
    
    float outerNormFactor = PI * (ra * ra - rb * rb);
    
    float innerNormFactor = PI * rb * rb;
    
    float n = 0.0;
    
    float m = 0.0;
    
    float2 resolution;
    
    prevState.GetDimensions(resolution.x, resolution.y);
    
    [unroll]
    for (float dx = -ra; dx <= ra; dx++)
    {
        [unroll]
        for (float dy = -ra; dy <= ra; dy++)
        {
            float radius = length(float2(dx, dy));
            float2 uv = fmod((float2(DTid) + float2(dx, dy)) / resolution, 1.0);
            float value = prevState.SampleLevel(samplerState, uv, 0.0);
            m += value * clampFactor(-radius, -rb, 1.0);
            n += value * clampFactor(-radius, -ra, 1.0) * clampFactor(radius, rb, 1.0);
        }
    }
    
    n /= outerNormFactor;
    
    m /= innerNormFactor;
    
    float color = s(n, m);
    
    curState[DTid] = color;
}