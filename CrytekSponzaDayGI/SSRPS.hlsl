Texture2D gViewPosition : register(t0);
Texture2D gNormal : register(t1); //convert to view space later
Texture2D originTexture : register(t2);

SamplerState wrapSampler : register(s0);
SamplerState clampSampler : register(s1);
SamplerComparisonState shadowSampler : register(s2);

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

//final output first two components is uv next two components is visibility
float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    const float2 textureSize = float2(1920.0, 1080.0);
    
    float4 positionFrom = gViewPosition.Sample(clampSampler, texCoord);
    
    float4 uv = float4(0.0, 0.0, 0.0, 0.0);
    
    if (positionFrom.w < 0.1)
    {
        return float4(0.0, 0.0, 0.0, 0.0);
    }
    
    const float3 unitPositionFrom = normalize(positionFrom.xyz);
    const float3 normal = normalize(mul(gNormal.Sample(clampSampler, texCoord).xyz, (float3x3) normalMatrix));
    const float3 pivot = normalize(reflect(unitPositionFrom, normal));
    
    float4 positionTo = positionFrom;
    
    float4 startView = float4(positionFrom.xyz, 1.0);
    
    float4 endView = float4(positionFrom.xyz + (pivot * maxDistance), 1.0);
    
    float4 startFrag = startView; //pixel location
    startFrag = mul(startFrag, proj);
    startFrag /= startFrag.w;
    startFrag.xy = startFrag.xy * float2(0.5, -0.5) + 0.5;
    startFrag.xy *= textureSize;
    
    float4 endFrag = endView; //pixel location
    endFrag = mul(endFrag, proj);
    endFrag /= endFrag.w;
    endFrag.xy = endFrag.xy * float2(0.5, -0.5) + 0.5;
    endFrag.xy *= textureSize;
    
    float2 frag = startFrag.xy;
    uv.xy = frag / textureSize;
    
    const float deltaX = endFrag.x - startFrag.x;
    const float deltaY = endFrag.y - startFrag.y;
    const float useX = abs(deltaX) >= abs(deltaY) ? 1.0 : 0.0;
    const float2 increment = float2(deltaX, deltaY) / 100.0;
    
    float search0 = 0.0;
    float search1 = 0.0;
    
    int hit0 = 0;
    int hit1 = 0;
    
    float viewDistance = startView.z;
    float depth = thickness;
    
    int i = 0;
    
    [unroll]
    for (i = 0; i < 100; ++i)
    {
        frag += increment;
        
        uv.xy = frag / textureSize;
        
        positionTo = gViewPosition.SampleLevel(clampSampler, uv.xy, 0.0);
        
        search1 = lerp((frag.y - startFrag.y) / deltaY, (frag.x - startFrag.x) / deltaX, useX);
        
        viewDistance = (startView.z * endView.z) / lerp(endView.z, startView.z, search1);
        
        depth = viewDistance - positionTo.z;
        
        if (depth > depthBias && depth < thickness)
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
    
    //if we find potential hit point then use binary search to find the closest hit location
    
    if (hit0)
    {
        [unroll]
        for (i = 0; i < 10; ++i)
        {
            frag = lerp(startFrag.xy, endFrag.xy, search1);
        
            uv.xy = frag / textureSize;
        
            positionTo = gViewPosition.SampleLevel(clampSampler, uv.xy, 0.0);
            
            viewDistance = (startView.z * endView.z) / lerp(endView.z, startView.z, search1);
        
            depth = viewDistance - positionTo.z;
        
            if (depth > depthBias && depth < thickness)
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
    
    float3 hitNormal = normalize(mul(gNormal.Sample(clampSampler, uv.xy).xyz, (float3x3) normalMatrix));
    
    float visibility =
      hit1
    * (1 - max(dot(-unitPositionFrom, pivot), 0))
    * (1 - clamp(depth / thickness, 0, 1))
    * (1 - clamp(length(positionTo - positionFrom) / maxDistance, 0, 1))
    * (uv.x >= 0.0 && uv.x <= 1.0)
    * (uv.y >= 0.0 && uv.y <= 1.0)
    * (dot(hitNormal, pivot) < 0.0 ? 1.0 : 0.0);
    
    visibility = clamp(visibility, 0.0, 1.0);
    
    uv.ba = float2(visibility, visibility);
    
    return uv;
}