#define FOVANGLEY 0.78539816339744
#define EPSILON 1e-3
#define MAXITERATION 16

cbuffer DeltaTime : register(b0)
{
    float dt;
    float sTime;
    uint uintSeed;
    float floatSeed;
}

cbuffer SimulationParam : register(b1)
{
    float phi;
    float theta;
    float RADIUS;
    float POWER;
}

class Ray
{
    float3 o;
    float3 d;
    
    float3 eval(float t)
    {
        return o + t * d;
    }
};

Ray CreateRay(float3 o,float3 d)
{
    Ray ray;
    ray.o = o;
    ray.d = d;
    return ray;
}

class Material
{
    float3 baseColor;
    float diffuse;
    float specular;
};

Material CreateMaterial(float3 baseColor, float diffuse, float specular)
{
    Material mat;
    mat.baseColor = baseColor;
    mat.diffuse = diffuse;
    mat.specular = specular;
    return mat;
}

class HitRecord
{
    float t;
    float3 n;
    Material mat;
};

class Sphere
{
    float r;
    float3 c;
    Material mat;
};

Sphere CreateSphere(float r, float3 c, Material mat)
{
    Sphere sphere;
    sphere.r = r;
    sphere.c = c;
    sphere.mat = mat;
    return sphere;
}

class Plane
{
    float3 n;
    Material mat;
};

HitRecord RayIntersect(Ray ray, Sphere sphere)
{
    float3 oc = ray.o - sphere.c;
    float l = dot(ray.d, oc);
    float det = l * l - dot(oc, oc) + sphere.r * sphere.r;
    
    HitRecord rec;
    rec.t = -1.0;
    rec.n = float3(0.0, 0.0, 0.0);
    rec.mat = CreateMaterial(float3(0.0, 0.0, 0.0), 0.0, 0.0);
    
    if (det < 0.0)
    {
        return rec;
    }
    
    float t = -l - sqrt(det);
    
    if (t < 0.0)
    {
        t = -l + sqrt(det);
    }
    
    if (t < 0.0)
    {
        return rec;
    }
    
    rec.t = t;
    rec.n = (ray.eval(t) - sphere.c) / sphere.r;
    rec.mat = sphere.mat;

    return rec;
}

static const float3 L = normalize(float3(-1.0, 0.75, 1.0));

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    float2 planePos = texCoord * 2.0 - 1.0;
    planePos.x *= 16.0 / 9.0;
    
    float3 cameraPos = float3(cos(phi) * cos(theta), cos(phi) * sin(theta), sin(phi)) * RADIUS;
    
    float3 cameraNormal = normalize(cameraPos);
    
    float3 helper = float3(0, 0, 1);
    
    float3 xVec = normalize(cross(cameraNormal, helper));
    float3 yVec = normalize(cross(cameraNormal, xVec));
    
    float3 rayDir = normalize(xVec * planePos.x + yVec * planePos.y - cameraNormal / tan(FOVANGLEY / 2.0));
    float3 rayOrigin = cameraPos;
    
    Ray ray = CreateRay(rayOrigin, rayDir);
    
    Material mat = CreateMaterial(float3(1.0, 1.0, 0.0), 1.0, 1.0);
    
    Sphere sphere = CreateSphere(1.0, float3(0.0, 0.0, 0.0), mat);
    
    HitRecord rec = RayIntersect(ray, sphere);
    
    if (rec.t < 0.0)
    {
        return float4(0.0, 0.0, 0.0, 1.0);
    }
    else
    {
        float3 diffuseColor = rec.mat.baseColor * max(dot(rec.n, L), 0.0);
        
        float3 R = reflect(ray.d, rec.n);
        
        float3 specularColor = float3(1.0, 1.0, 1.0) * pow(max(dot(R, L), 0.0), 32.0);
        
        return float4(diffuseColor + specularColor, 1.0);
    }
}