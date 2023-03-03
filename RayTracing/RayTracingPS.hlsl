#define FOVANGLEY 0.78539816339744
#define EPSILON 1e-3
#define MAXITERATION 32

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

Ray CreateRay(float3 o, float3 d)
{
    Ray ray;
    ray.o = o;
    ray.d = d;
    return ray;
}

class Material
{
    float3 color;
    float diffuseFactor;
    float specularFactor;
};

Material CreateMaterial(float3 color, float diffuseFactor, float specularFactor)
{
    Material mat;
    mat.color = color;
    mat.diffuseFactor = diffuseFactor;
    mat.specularFactor = specularFactor;
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
    float3 c;
    float r;
    Material mat;
};

Sphere CreateSphere(float3 c, float r, Material mat)
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

Plane CreatePlane(float3 n, Material mat)
{
    Plane plane;
    plane.n = n;
    plane.mat = mat;
    return plane;
}

HitRecord RayIntersect(in Ray ray, in Sphere sphere)
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

HitRecord RayIntersect(in Ray ray, in Plane plane)
{
    float len = -dot(ray.o, plane.n) / dot(ray.d, plane.n);
    
    HitRecord rec;
    rec.t = -1.0;
    rec.n = float3(0.0, 0.0, 0.0);
    rec.mat = CreateMaterial(float3(0.0, 0.0, 0.0), 0.0, 0.0);
    
    if (len < 0.0)
    {
        return rec;
    }
    
    rec.t = len;
    rec.n = plane.n;
    rec.mat = plane.mat;
    
    return rec;
}

HitRecord GetCloseHit(in HitRecord closeHit, in HitRecord hit)
{
    if (closeHit.t < 0.0 || (hit.t > 0.0 && hit.t < closeHit.t))
    {
        return hit;
    }
    
    return closeHit;
}

static const float3 ambient = float3(0.6, 0.8, 1.0);
static const float3 L = normalize(float3(-1.0, 0.75, 1.0));
static const Sphere spheres[4] =
{
    CreateSphere(float3(0.0, 0.0, 1.2), 1.0, CreateMaterial(float3(1.0, 1.0, 0.0), 1.0, 0.5)),
    CreateSphere(float3(3.0 * cos(sTime), 3.0 * sin(sTime), 1.2), 1.0, CreateMaterial(float3(1.0, 0.0, 0.0), 1.0, 0.1)),
    CreateSphere(float3(-4.0, -4.0, 2.0 + sin(sTime)), 1.0, CreateMaterial(float3(0.0, 1.0, 0.0), 1.0, 0.2)),
    CreateSphere(float3(4.0 + cos(sTime), -4.0, 1.5), 1.0, CreateMaterial(float3(0.0, 0.0, 1.0), 1.0, 0.3)),
};

//找到最近的t
HitRecord Trace(Ray ray)
{
    HitRecord closeHit = RayIntersect(ray, CreatePlane(float3(0.0, 0.0, 1.0), CreateMaterial(float3(1.0, 1.0, 1.0), 1.0, 0.0)));
    
    [unroll]
    for (uint i = 0; i < 4; i++)
    {
        HitRecord hit = RayIntersect(ray, spheres[i]);
        closeHit = GetCloseHit(closeHit, hit);
    }
    
    return closeHit;
}

float3 Radiance(Ray ray)
{
    float3 color = float3(0.0, 0.0, 0.0);
    float3 fresnel = float3(0.0, 0.0, 0.0);
    float3 mask = float3(1.0, 1.0, 1.0);
    
    [loop]
    for (int i = 0; i < MAXITERATION; i++)
    {
        HitRecord hit = Trace(ray);
        
        if (hit.t > 0.0)
        {
            float3 r0 = hit.mat.color * hit.mat.specularFactor;
            float hv = saturate(dot(hit.n, -ray.d));
            fresnel = r0 + (1.0 - r0) * pow(1.0 - hv, 5.0);
            mask *= fresnel;

            if (Trace(CreateRay(ray.eval(hit.t) + EPSILON * L, L)).t < 0.0)
            {
                color += max(dot(hit.n, L), 0.0) * hit.mat.color * hit.mat.diffuseFactor * (1.0 - fresnel) * mask / fresnel;
            }
            
            float3 reflection = reflect(ray.d, hit.n);
            ray = CreateRay(ray.eval(hit.t) + EPSILON * reflection, reflection);
        }
        else
        {
            float3 skyColor = float3(1.0, 1.0, 1.0) * pow(max(dot(ray.d, L), 0.0), 16.0);
            color += (ambient + skyColor) * mask;
            break;
        }
    }
    
    return color;
}

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    float2 planePos = texCoord * 2.0 - 1.0;
    planePos.x *= 16.0 / 9.0;
    
    float3 cameraPos = float3(cos(phi) * cos(theta), cos(phi) * sin(theta), sin(phi)) * RADIUS;
    
    float3 helper = float3(0, 0, 1);
    
    float3 xVec = normalize(cross(helper, cameraPos));
    float3 yVec = normalize(cross(xVec, cameraPos));
    
    float3 rayDir = normalize(xVec * planePos.x + yVec * planePos.y - normalize(cameraPos) / tan(FOVANGLEY / 2.0));
    float3 rayOrigin = cameraPos;
    
    Ray ray = CreateRay(rayOrigin, rayDir);
    
    return float4(Radiance(ray), 1.0);
}