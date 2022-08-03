#define ITERATIONS 200

SamplerState linearSampler : register(s0);

Texture2D tNoise : register(t0);
Texture2D tDust : register(t1);
Texture2D tLast : register(t2);

cbuffer DeltaTimes : register(b0)
{
    float deltaTime;
    float sTime;
    float v2;
    float v3;
};

float noise(in float3 x)
{
    const float3 p = floor(x);
    float3 f = frac(x);
    f = f * f * (float3(3.0, 3.0, 3.0) - float3(2.0, 2.0, 2.0) * f);
    const float2 uv = (p.xy + float2(37.0, 17.0) * p.z) + f.xy;
    const float2 rg = tNoise.Sample(linearSampler, (uv + float2(0.5, 0.5)) / 256.0).yx;
    return -1.0 + 2.0 * lerp(rg.x, rg.y, f.z);
}

float rand(float2 coord)
{
    return saturate(frac(sin(dot(coord, float2(12.9898, 78.223))) * 43758.5453));
}

float pcurve(float x, float a, float b)
{
    const float k = pow(a + b, a + b) / (pow(a, a) * pow(b, b));
    return k * pow(x, a) * pow(1.0 - x, b);
}

float sdTorus(float3 p, float2 t)
{
    const float2 q = float2(length(p.xz) - t.x, p.y);
    return length(q) - t.y;
}

void Haze(inout float3 color, float3 pos, float alpha)
{
    const float2 t = float2(1.0, 0.01);

    const float torusDist = abs(sdTorus(pos + float3(0.0, -0.05, 0.0), t));

    float bloomDisc = 1.0 / (pow(torusDist, 2.0) + 0.001);
    bloomDisc *= length(pos) < 0.5 ? 0.0 : 1.0;

    const float d = bloomDisc * (2.9 / float(ITERATIONS)) * (1.0 - alpha * 1.0);
    
    color += float3(d, d, d);
}

void GasDisc(inout float3 color, inout float alpha, float3 pos)
{
    const float discRadius = 3.2;
    const float discWidth = 5.3;
    const float discInner = discRadius - discWidth * 0.5;
    
    float discThickness = 0.1;

    const float distFromCenter = length(pos);
    const float distFromDisc = pos.y;
    
    const float radialGradient = 1.0 - saturate((distFromCenter - discInner) / discWidth * 0.5);

    float coverage = pcurve(radialGradient, 4.0, 0.9);

    discThickness *= radialGradient;
    coverage *= saturate(1.0 - abs(distFromDisc) / discThickness);

    const float3 dustColorLit = float3(1.0, 1.0, 1.0);

    const float dustGlow = 1.0 / (pow(1.0 - radialGradient, 2.0) * 290.0 + 0.002);
    float3 dustColor = dustColorLit * dustGlow * 8.2;

    coverage = saturate(coverage * 0.7);


    const float fade = pow((abs(distFromCenter - discInner) + 0.4), 4.0) * 0.04;
    const float bloomFactor = 1.0 / (pow(distFromDisc, 2.0) * 40.0 + fade + 0.00002);
    float3 b = dustColorLit * pow(bloomFactor, 1.5);
    
    b *= lerp(float3(1.7, 1.1, 1.0), float3(0.5, 0.6, 1.0), float3(pow(radialGradient, 2.0), pow(radialGradient, 2.0), pow(radialGradient, 2.0)));
    b *= lerp(float3(1.7, 0.5, 0.1), float3(1.0, 1.0, 1.0), float3(pow(radialGradient, 0.5), pow(radialGradient, 0.5), pow(radialGradient, 0.5)));

    dustColor = lerp(dustColor, b * 150.0, saturate(1.0 - coverage * 1.0));
    coverage = saturate(coverage + bloomFactor * bloomFactor * 0.1);
    
    if (coverage < 0.01)
    {
        return;
    }
    
    
    float3 radialCoords;
    radialCoords.x = distFromCenter * 1.5 + 0.55;
    radialCoords.y = atan2(-pos.x, -pos.z) * 1.5;
    radialCoords.z = distFromDisc * 1.5;

    radialCoords *= 0.95;
    
    const float speed = 0.20;
    
    float noise1 = 1.0;
    float3 rc = radialCoords;
    rc.y += sTime * speed;
    noise1 *= noise(rc * 3.0) * 0.5 + 0.5;
    rc.y -= sTime * speed;
    noise1 *= noise(rc * 6.0) * 0.5 + 0.5;
    rc.y += sTime * speed;
    noise1 *= noise(rc * 12.0) * 0.5 + 0.5;
    rc.y -= sTime * speed;
    noise1 *= noise(rc * 24.0) * 0.5 + 0.5;
    rc.y += sTime * speed;

    float noise2 = 2.0;
    rc = radialCoords + float3(30.0, 30.0, 30.0);
    noise2 *= noise(rc * 3.0) * 0.5 + 0.5;
    rc.y += sTime * speed;
    noise2 *= noise(rc * 6.0) * 0.5 + 0.5;
    rc.y -= sTime * speed;
    noise2 *= noise(rc * 12.0) * 0.5 + 0.5;
    rc.y += sTime * speed;
    noise2 *= noise(rc * 24.0) * 0.5 + 0.5;
    rc.y -= sTime * speed;
    noise2 *= noise(rc * 48.0) * 0.5 + 0.5;
    rc.y += sTime * speed;
    noise2 *= noise(rc * 92.0) * 0.5 + 0.5;

    dustColor *= noise1 * 0.998 + 0.002;
    coverage *= noise2;
    
    radialCoords.y += sTime * speed * 0.5;
    
    dustColor *= pow(tDust.Sample(linearSampler, radialCoords.yx * float2(0.15, 0.27)).rgb, float3(2.0, 2.0, 2.0)) * 4.0;
    
    coverage = saturate(coverage * 1200.0 / float(ITERATIONS));
    dustColor = max(float3(0.0, 0.0, 0.0), dustColor);
    
    coverage *= pcurve(radialGradient, 4.0, 0.9);
    
    color += (1.0 - alpha) * dustColor * coverage;

    alpha += (1.0 - alpha) * coverage;
}

float3 rotate(float3 p, float x, float y, float z)
{
    const float3x3 matx = float3x3(
    1.0, 0.0, 0.0,
    0.0, cos(x), -sin(x),
    0.0, sin(x), cos(x)
    );
    
    const float3x3 maty = float3x3(
    cos(y), 0.0, sin(y),
    0.0, 1.0, 0.0,
    -sin(y), 0.0, cos(y)
    );
    
    const float3x3 matz = float3x3(
    cos(z), -sin(z), 0.0,
    sin(z), cos(z), 0.0,
    0.0, 0.0, 1.0
    );
    
    p = mul(p, matx);
    p = mul(p, matz);
    p = mul(p, maty);
    
    return p;
}

void RotateCamera(inout float3 eyevec, inout float3 eyepos)
{
    const float mousePosY = 1.0;
    const float mousePosX = 0.5;

    const float3 angle = float3(mousePosY * 0.05 + 0.05, 1.0 + mousePosX * 1.0, -0.45);

    eyevec = rotate(eyevec, angle.x, angle.y, angle.z);
    eyepos = rotate(eyepos, angle.x, angle.y, angle.z);
}

void WarpSpace(inout float3 eyevec, inout float3 raypos)
{
    const float3 origin = float3(0.0, 0.0, 0.0);

    const float singularityDist = distance(raypos, origin);
    const float warpFactor = 1.0 / (pow(singularityDist, 2.0) + 0.000001);

    const float3 singularityVector = normalize(origin - raypos);
    
    const float warpAmount = 5.0;

    eyevec = normalize(eyevec + singularityVector * warpFactor * warpAmount / float(ITERATIONS));
}

float4 main(float2 texCoord:TEXCOORD) : SV_Target
{
    float2 uveye = texCoord;
    
    uveye.x += (rand(texCoord + sin(sTime * 1.0)) / 1920.0) * 1.0;
    uveye.y += (rand(texCoord + 1.0 + sin(sTime * 1.0)) / 1080.0) * 1.0;
    
    float3 eyevec = normalize(float3((uveye * 2.0 - 1.0) * float2(16.0 / 9.0, 1.0), 6.0));
    float3 eyepos = float3(0.0, -0.0, -10.0);
    
    float2 mousepos = float2(0.5, 1.0);
    eyepos.x += mousepos.x * 3.0 - 1.5;
    
    const float far = 15.0;

    RotateCamera(eyevec, eyepos);
    
    const float dither = rand(texCoord + sin(sTime * 1.0)) * 2.0;

    float3 raypos = eyepos + eyevec * dither * far / float(ITERATIONS);
    
    float3 color = float3(0.0, 0.0, 0.0);
    float alpha = 0.0;
    
    for (int i = 0; i < ITERATIONS; i++)
    {
        WarpSpace(eyevec, raypos);
        raypos += eyevec * far / float(ITERATIONS);
        GasDisc(color, alpha, raypos);
        Haze(color, raypos, alpha);
    }
    
    const float colorFactor = 200.0;
    
    color *= 0.0001 * colorFactor;
    
    color = pow(color, float3(1.5, 1.5, 1.5));
    color = color / (1.0 + color);
    color = pow(color, float3(1.0 / 1.5, 1.0 / 1.5, 1.0 / 1.5));

    
    color = lerp(color, color * color * (3.0 - 2.0 * color), float3(1.0, 1.0, 1.0));
    color = pow(color, float3(1.3, 1.20, 1.0));

    color = saturate(color * 1.01);
    
    color = pow(color, float3(0.7 / 2.2, 0.7 / 2.2, 0.7 / 2.2));
    
    const float p = 1.0;
    float3 previous = pow(tLast.Sample(linearSampler, texCoord).rgb, float3(1.0 / p, 1.0 / p, 1.0 / p));
    
    color = pow(color, float3(1.0 / p, 1.0 / p, 1.0 / p));
    
    const float blendWeight = 0.9 * 1.0;
    
    color = lerp(color, previous, blendWeight);
    
    color = pow(color, float3(p, p, p));
    
    return float4(color, 1.0);
}