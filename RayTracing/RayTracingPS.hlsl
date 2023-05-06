#define FOVANGLEY 0.78539816339744
#define BIG_EPSILON 1e-3
#define SMALL_EPSILON 1e-8
#define PI         3.1415926535
#define TWO_PI     6.2831853071

Texture2D historyTex : register(t0);
SamplerState linearSampler : register(s0);

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

cbuffer TemporalAccumulationParam : register(b2)
{
    uint frameCount;
    float randomSeed;
    float2 padding;
}

static float hashSeed = 0.0;

uint BaseHash(uint2 p)
{
    p = 1103515245U * ((p >> 1U) ^ (p.yx));
    uint h32 = 1103515245U * ((p.x) ^ (p.y >> 3U));
    return h32 ^ (h32 >> 16);
}

float Hash1(inout float seed)
{
    uint n = BaseHash(asuint(float2(seed += 0.1, seed += 0.1)));
    return float(n) * (1.0 / float(0xffffffffU));
}

float2 Hash2(inout float seed)
{
    uint n = BaseHash(asuint(float2(seed += 0.1, seed += 0.1)));
    uint2 rz = uint2(n, n * 48271U);
    return float2(rz.xy & uint2(0x7fffffffU, 0x7fffffffU)) / float(0x7fffffff);
}

float3 Hash3(inout float seed)
{
    uint n = BaseHash(asuint(float2(seed += 0.1, seed += 0.1)));
    uint3 rz = uint3(n, n * 16807U, n * 48271U);
    return float3(rz & uint3(0x7fffffffU, 0x7fffffffU, 0x7fffffffU)) / float(0x7fffffff);
}

float3 RandomPointInUnitSphere(inout float seed)
{
    float3 h = Hash3(seed) * float3(TWO_PI, 2.0, 1.0) - float3(0.0, 1.0, 0.0);
    float theta = h.x;
    float sinPhi = sqrt(1.0 - h.y * h.y);
    float r = pow(h.z, 0.3333333334);
    
    return r * float3(cos(theta) * sinPhi, sin(theta) * sinPhi, h.y);
}

class Ray
{
    float3 o;
    float3 d;
    
    float3 evaluate(in float t)
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
    int type;
    float3 baseColor;
    float param;//metal fuzz dielectric refractive index
};

#define MATERIAL_LAMBERTIAN 0
#define MATERIAL_DIELECTRIC 1
#define MATERIAL_METAL 2

Material CreateMaterial(in int type, in float3 baseColor, in float param)
{
    Material material;
    material.type = type;
    material.baseColor = baseColor;
    material.param = param;
    return material;
}

class HitRecord
{
    float t;
    float3 n;
    Material material;
};

class Sphere
{
    float3 c;
    float r;
    Material material;
};

Sphere CreateSphere(in float3 c,in float r,in Material material)
{
    Sphere sphere;
    sphere.c = c;
    sphere.r = r;
    sphere.material = material;
    return sphere;
}

float Schlick(in float cosine, in float refractIndex)
{
    float r0 = (1.0 - refractIndex) / (1.0 + refractIndex);
    r0 = r0 * r0;
    return r0 + (1.0 - r0) * pow((1.0 - cosine), 5.0);
}

void RayIntersect(in Ray ray, in Sphere sphere, inout HitRecord rec, inout bool hitObject)
{
    float3 oc = ray.o - sphere.c;
    float l = dot(ray.d, oc);
    float det = l * l - dot(oc, oc) + sphere.r * sphere.r;
    
    if (det < 0.0)
    {
        return;
    }
    
    float t = -l - sqrt(det);
    
    if (t < BIG_EPSILON)
    {
        t = -l + sqrt(det);
        if (t < BIG_EPSILON)
        {
            return;
        }
    }
    
    if(rec.t < t)
    {
        return;
    }
    
    hitObject = true;
    rec.t = t;
    rec.n = (ray.evaluate(t) - sphere.c) / sphere.r;
    rec.material = sphere.material;

    return;
}

static const Sphere spheres[24] =
{
    CreateSphere(float3(0.0, -1000., -1.0), 1000.0, CreateMaterial(MATERIAL_LAMBERTIAN, float3(0.5, 0.5, 0.5), 0.0)),
    CreateSphere(float3(-4.0, 1.0, 2.0), 1.0, CreateMaterial(MATERIAL_DIELECTRIC, float3(1.0, 1.0, 1.0), 1.5)),
    CreateSphere(float3(0.0, 1.0, 0.0), 1.0, CreateMaterial(MATERIAL_METAL, float3(0.7, 0.6, 0.5), 0.0)),
    CreateSphere(float3(4.0, 1.0, 2.0), 1.0, CreateMaterial(MATERIAL_LAMBERTIAN, float3(0.7, 0.3, 0.3), 0.0)),
    CreateSphere(float3(-6.0, 0.2, 2.8), 0.2, CreateMaterial(MATERIAL_DIELECTRIC, float3(1.0, 1.0, 1.0), 1.5)),
    CreateSphere(float3(1.6, 0.2, -0.9), 0.2, CreateMaterial(MATERIAL_DIELECTRIC, float3(1.0, 1.0, 1.0), 1.5)),
    CreateSphere(float3(-5.7, 0.2, -2.7), 0.2, CreateMaterial(MATERIAL_LAMBERTIAN, float3(0.8, 0.3, 0.3), 0.0)),
    CreateSphere(float3(-3.6, 0.2, -4.4), 0.2, CreateMaterial(MATERIAL_LAMBERTIAN, float3(0.9, 0.3, 0.2), 0.0)),
    CreateSphere(float3(0.8, 0.2, 2.3), 0.2, CreateMaterial(MATERIAL_LAMBERTIAN, float3(0.2, 0, 0.5), 0.0)),
    CreateSphere(float3(3.8, 0.2, 4.2), 0.2, CreateMaterial(MATERIAL_LAMBERTIAN, float3(0.4, 0.3, 0.7), 0.0)),
    CreateSphere(float3(-0.1, 0.2, -1.9), 0.2, CreateMaterial(MATERIAL_LAMBERTIAN, float3(0.4, 0, 0.4), 0.0)),
    CreateSphere(float3(-2.5, 0.2, 5.4), 0.2, CreateMaterial(MATERIAL_METAL, float3(0.3, 0.7, 0.9), 0.3)),
    CreateSphere(float3(-3.9, 0.2, -0.3), 0.2, CreateMaterial(MATERIAL_LAMBERTIAN, float3(0.9, 0.8, 0.5), 0.0)),
    CreateSphere(float3(-6.0, 0.2, 4.0), 0.2, CreateMaterial(MATERIAL_LAMBERTIAN, float3(0.9, 0.9, 0.5), 0.0)),
    CreateSphere(float3(4.4, 0.2, -0.5), 0.2, CreateMaterial(MATERIAL_LAMBERTIAN, float3(0.5, 0.4, 0.8), 0.0)),
    CreateSphere(float3(3.4, 0.2, 5.3), 0.2, CreateMaterial(MATERIAL_LAMBERTIAN, float3(0.1, 0.6, 0.2), 0.0)),
    CreateSphere(float3(4.6, 0.2, -3.8), 0.2, CreateMaterial(MATERIAL_LAMBERTIAN, float3(0.2, 0.2, 0.2), 0.0)),
    CreateSphere(float3(0.7, 0.2, -2.5), 0.2, CreateMaterial(MATERIAL_METAL, float3(0, 0.2, 0.1), 0.0)),
    CreateSphere(float3(2.4, 0.2, -4.3), 0.2, CreateMaterial(MATERIAL_LAMBERTIAN, float3(0.8, 0.9, 0), 0.0)),
    CreateSphere(float3(4.4, 0.2, 4.9), 0.2, CreateMaterial(MATERIAL_LAMBERTIAN, float3(0.8, 0.8, 0), 0.0)),
    CreateSphere(float3(-4.7, 0.2, 4.6), 0.2, CreateMaterial(MATERIAL_LAMBERTIAN, float3(0.8, 0.8, 0.7), 0.0)),
    CreateSphere(float3(4.2, 0.2, -3.5), 0.2, CreateMaterial(MATERIAL_LAMBERTIAN, float3(0.8, 0.8, 0.6), 0.0)),
    CreateSphere(float3(-5.2, 0.2, 0.5), 0.2, CreateMaterial(MATERIAL_LAMBERTIAN, float3(0.2, 0.7, 0.9), 0.0)),
    CreateSphere(float3(5.7, 0.2, -0.8), 0.2, CreateMaterial(MATERIAL_LAMBERTIAN, float3(0.3, 0, 0.7), 0.0))
};

float3 GetBackgroundColor(in float y)
{
    return lerp(float3(1.0, 1.0, 1.0), float3(0.5, 0.7, 1.0), y);
}

bool Trace(in Ray ray, out HitRecord rec)
{
    bool hit = false;
    
    rec.t = 99999.0;
    rec.n = float3(0.0, 0.0, 0.0);
    rec.material = CreateMaterial(MATERIAL_LAMBERTIAN, float3(0.0, 0.0, 0.0), 0.0);
    
    [unroll]
    for (uint i = 0; i < 24; i++)
    {
        RayIntersect(ray, spheres[i], rec, hit);
    }
    
    return hit;
}

void FlipNormalAndIOR(in Ray ray, inout HitRecord rec)
{
    float isFrontFace = float(dot(ray.d, rec.n) < 0.0);
    rec.material.param = lerp(rec.material.param, 1.0 / rec.material.param, isFrontFace);
    rec.n = lerp(-rec.n, rec.n, isFrontFace);
}

bool RefractPBRT(in float3 V, in float3 N, in float refractIndex, inout float cosThetaI, inout float3 transmitted)
{
    transmitted = float3(SMALL_EPSILON, SMALL_EPSILON, SMALL_EPSILON);
    
    cosThetaI = max(0.0, dot(-V, N));
    float sin2ThetaI = 1.0 - cosThetaI * cosThetaI;
    float cos2ThetaT = 1.0 - refractIndex * refractIndex * sin2ThetaI;
    
    if (cos2ThetaT < 0.0)
        return false;
    
    transmitted = refractIndex * V + (refractIndex * cosThetaI - sqrt(cos2ThetaT)) * N;
    
    return true;
}

bool ScatterRay(in Ray ray, in HitRecord rec, inout float3 attenuation, inout Ray scattered)
{
    if (rec.material.type == MATERIAL_LAMBERTIAN)
    {
        scattered = CreateRay(ray.evaluate(rec.t), normalize(rec.n + RandomPointInUnitSphere(hashSeed)));
        attenuation = rec.material.baseColor;
        return true;
    }
    else if (rec.material.type == MATERIAL_DIELECTRIC)
    {
        FlipNormalAndIOR(ray, rec);
        float3 transmitDirection;

        float cosThetaI;
        bool isRefracted = RefractPBRT(ray.d, rec.n, rec.material.param, cosThetaI, transmitDirection);
        float shouldReflect = lerp(1.0, step(Hash1(hashSeed), Schlick(cosThetaI, rec.material.param)), float(isRefracted));
        transmitDirection = lerp(transmitDirection, reflect(ray.d, rec.n), shouldReflect);
        
        scattered = CreateRay(ray.evaluate(rec.t), normalize(transmitDirection));
        
        attenuation = rec.material.baseColor;
        
        return true;
    }
    else if (rec.material.type == MATERIAL_METAL)
    {
        scattered = CreateRay(ray.evaluate(rec.t), normalize(reflect(ray.d, rec.n) + rec.material.param * RandomPointInUnitSphere(hashSeed)));
        attenuation = rec.material.baseColor;
        return dot(scattered.d, rec.n) > 0.0;
    }
    
    scattered = CreateRay(float3(0.0, 0.0, 0.0), float3(0.0, 0.0, 0.0));
    attenuation = float3(0.0, 0.0, 0.0);
    return false;
}

float3 Radiance(Ray ray)
{
    float3 color = float3(1.0, 1.0, 1.0);
    
    HitRecord rec;
    
    [unroll]
    for (uint i = 0; i < 8; i++)
    {
        if (Trace(ray, rec))
        {
            Ray scattered;
            float3 attenuation;
            if (ScatterRay(ray, rec, attenuation, scattered))
            {
                color *= attenuation;
                ray = scattered;
            }
            else
            {
                break;
            }
        }
        else
        {
            return color * GetBackgroundColor(ray.d.y * 0.5 + 0.5);
        }
    }
    
    return float3(0.0, 0.0, 0.0);
}

float4 main(float2 texCoord : TEXCOORD, float4 pixelCoord : SV_POSITION) : SV_TARGET
{
    hashSeed = float(BaseHash(asuint(pixelCoord.xy))) / float(0xffffffffU) + randomSeed;
    
    float2 planePos = (floor(pixelCoord.xy) + Hash2(hashSeed)) / float2(1920.0, 1080.0) * 2.0 - 1.0;
    planePos.x *= 16.0 / 9.0;
    
    float3 cameraPos = float3(cos(phi) * cos(theta), sin(phi), cos(phi) * sin(theta)) * RADIUS;
    
    float3 helper = float3(0.0, 1.0, 0.0);
    
    float3 xVec = normalize(cross(helper, cameraPos));
    float3 yVec = normalize(cross(xVec, cameraPos));
    
    float3 rayDir = normalize(xVec * planePos.x + yVec * planePos.y - normalize(cameraPos) / tan(FOVANGLEY / 2.0));
    float3 rayOrigin = cameraPos;
    
    Ray ray = CreateRay(rayOrigin, rayDir);
    
    return float4(Radiance(ray), 1.0 / float(frameCount));
}
