Texture2D<float4> gPosition : register(t0);
Texture2D<float4> gNormalSpecular : register(t1);
Texture2D<float> depthTexture : register(t2);
Texture2D<float4> originTexture : register(t3);

SamplerState wrapSampler : register(s0);
SamplerState clampSampler : register(s1);
SamplerComparisonState shadowSampler : register(s2);

#define RAYMARCHSTEP 200.0

cbuffer ProjMatrix : register(b1)
{
    matrix proj;
}

cbuffer ViewMatrix : register(b2)
{
    matrix view;
    float4 viewPos;
    matrix prevViewProj;
    matrix viewProj;
    matrix normalMatrix;
};

cbuffer SSRParam : register(b3)
{
    float maxDistance;
    float thickness;
    float depthBias;
    float padding;
}

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    float4 pos = gPosition.Sample(clampSampler, texCoord);
    
    float4 color = originTexture.Sample(clampSampler, texCoord);
    
    if (pos.w < 0.5)
    {
        return color;
    }
    
    float4 positionFrom = mul(pos, view);
    
    float4 normalSpeculr = gNormalSpecular.Sample(clampSampler, texCoord);
    
    const float3 unitPositionFrom = normalize(positionFrom.xyz);
    const float3 normal = normalize(mul(normalSpeculr.xyz, (float3x3) normalMatrix));
    const float3 pivot = normalize(reflect(unitPositionFrom, normal));
    
    float4 startView = float4(positionFrom.xyz, 1.0);
    
    float4 endView = float4(positionFrom.xyz + (pivot * maxDistance), 1.0);
    
    float4 startFrag = mul(startView, proj);
    startFrag /= startFrag.w;
    startFrag.xy = startFrag.xy * float2(0.5, -0.5) + 0.5;
    
    float4 endFrag = mul(endView, proj);
    endFrag /= endFrag.w;
    endFrag.xy = endFrag.xy * float2(0.5, -0.5) + 0.5;
    
    float3 percentage = (saturate(endFrag.xyz) - startFrag.xyz) / (endFrag.xyz - startFrag.xyz);
    
    float minPercentage = saturate(min(min(percentage.x, percentage.y), percentage.z));
    
    float3 increment = (endFrag.xyz - startFrag.xyz) / RAYMARCHSTEP;
    
    float3 curUV = startFrag.xyz;
    
    int i = 0;
    
    int hit0 = 0;
   
    int hit1 = 0;
    
    float search0 = 0.0;
    
    float search1 = 0.0;
    
    float depthDiff = thickness;
    
    [loop]
    for (i = 0; i < int(RAYMARCHSTEP * minPercentage); ++i)
    {
        curUV += increment;
        
        search1 = float(i + 1) / RAYMARCHSTEP;
        
        float frontDepth = depthTexture.SampleLevel(clampSampler, curUV.xy, 0.0);
        
        depthDiff = curUV.z - frontDepth;
        
        if (depthDiff > 0.0 && depthDiff <= thickness)
        {
            hit0 = 1;
            break;
        }
        else
        {
            search0 = search1;
        }
    }
    
    search1 = search0 + ((search1 - search0) / 2.0);
    
    if (hit0)
    {
        [unroll]
        for (i = 0; i < 10; ++i)
        {
            curUV = lerp(startFrag.xyz, endFrag.xyz, search1);
        
            float frontDepth = depthTexture.SampleLevel(clampSampler, curUV.xy, 0.0);
        
            depthDiff = curUV.z - frontDepth;
        
            if (depthDiff > 0.0 && depthDiff <= thickness)
            {
                hit1 = 1;
                search1 = search0 + ((search1 - search0) / 2.0);
            }
            else
            {
                float temp = search1;
                search1 = search1 + ((search1 - search0) / 2.0);
                search0 = temp;
            }
        }
    }
    
    float4 positionTo = mul(gPosition.Sample(clampSampler, curUV.xy), view);
    
    float3 hitNormal = normalize(mul(gNormalSpecular.Sample(clampSampler, curUV.xy).xyz, (float3x3) normalMatrix));
    
    float visibility =
      hit1
    * (1 - max(dot(-unitPositionFrom, pivot), 0))
    * (1 - clamp(depthDiff / thickness, 0, 1))
    * (1 - clamp(length(positionTo - positionFrom) / maxDistance, 0, 1))
    * (dot(hitNormal, pivot) < 0.0 ? 1.0 : 0.0);
    
    visibility = clamp(visibility, 0.0, 1.0);
    
    color.rgb += originTexture.Sample(clampSampler, curUV.xy).rgb * visibility * normalSpeculr.w;
    
    return color;
}