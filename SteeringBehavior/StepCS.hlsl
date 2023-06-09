RWBuffer<float4> positionVelocity : register(u0);

Buffer<float4> maxSpeedMaxForce : register(t0);

cbuffer DeltaTime : register(b0)
{
    float dt;
    float sTime;
    uint uintSeed;
    float floatSeed;
}

cbuffer SimulationParam : register(b1)
{
    uint vehicleNum;
    float speedMultiply;
    float v0;
    float v1;
};

static float2 position;

static float2 velocity;

static float mSpeed;

static float mForce;

void limitMag(inout float2 v, float maxMag)
{
    float mag = length(v);

    mag = clamp(mag, 0.0, maxMag);
    
    v = normalize(v) * mag;
}

void setMag(inout float2 v, float mag)
{
    v = normalize(v) * mag;
}

float2 seek(float2 target)
{
    float2 force = target - position;
    setMag(force, mSpeed);
    force -= velocity;
    limitMag(force, mForce);
    return force;
}

float2 separate()
{
    float desiredSeparation = 25.f;
    
    float2 steer = float2(0, 0);
    
    uint count = 0;
    
    for (uint i = 0; i < vehicleNum; i++)
    {
        float d = length(position - positionVelocity[i].xy);
        if ((d > 0) && (d < desiredSeparation))
        {
            float2 diff = normalize(position - positionVelocity[i].xy) / d;
            steer += diff;
            count++;
        }
    }
    
    if (count > 0)
        steer /= float(count);
    
    if (length(steer) > 0)
    {
        setMag(steer, mSpeed);
        steer -= velocity;
        limitMag(steer, mForce);
    }
    
    return steer;
}

float2 align()
{
    float neighborDist = 50.f;
    
    float2 sum = float2(0, 0);
    
    uint count = 0;
    
    for (uint i = 0; i < vehicleNum; i++)
    {
        float d = length(position - positionVelocity[i].xy);
        if ((d > 0) && (d < neighborDist))
        {
            sum += positionVelocity[i].zw;
            count++;
        }
    }

    if (count > 0)
    {
        setMag(sum, mSpeed);
        float2 steer = sum - velocity;
        limitMag(steer, mForce);
        return steer;
    }
    else
    {
        return float2(0, 0);
    }
}

float2 cohesion()
{
    float neighborDist = 50.f;
    
    float2 sum = float2(0, 0);
    
    uint count = 0;
    
    for (uint i = 0; i < vehicleNum; i++)
    {
        float d = length(position - positionVelocity[i].xy);
        if ((d > 0) && (d < neighborDist))
        {
            sum += positionVelocity[i].xy;
            count++;
        }
    }

    if (count > 0)
    {
        sum /= float(count);
        return seek(sum);
    }
    else
    {
        return float2(0, 0);
    }
}

[numthreads(1000, 1, 1)]
void main(uint DTid : SV_DispatchThreadID)
{
    position = positionVelocity[DTid].xy;
    
    velocity = positionVelocity[DTid].zw;
    
    mSpeed = maxSpeedMaxForce[DTid].x;
    
    mForce = maxSpeedMaxForce[DTid].y;
    
    float2 sep = separate() * 2.0;
    float2 ali = align();
    float2 coh = cohesion();
    
    AllMemoryBarrierWithGroupSync();
    
    float2 acceleration = sep + ali + coh;
    
    float scaledDt = dt * speedMultiply;
    
    velocity += acceleration * scaledDt;
    
    limitMag(velocity, mSpeed);
    
    position += velocity * scaledDt;
    
    if (position.x < -40)
    {
        position.x = 1960;
    }
    else if (position.x > 1960)
    {
        position.x = -40;
    }
    
    if (position.y < -40)
    {
        position.y = 1120;
    }
    else if (position.y > 1120)
    {
        position.y = -40;
    }
    
    positionVelocity[DTid] = float4(position, velocity);
}