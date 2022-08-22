#pragma once

#include<vector>

#include"Vector.h"

class Vehicle
{
public:

	Vector prePos;
	Vector pos;
	Vector vel;
	Vector acc;

	float mSpeed;
	float mForce;
	//float mass;

	float r;
	float g;
	float b;

	Vehicle(const Vector& pos, const Vector& vel, const float& mSpeed, const float& mForce, const float& r, const float& g, const float& b) :
		pos(pos), prePos(pos), vel(vel), acc(Vector(0, 0)), mSpeed(mSpeed), mForce(mForce), r(r), g(g), b(b)
	{
	}

	void flock(std::vector<Vehicle>& vehicles);

	Vector separate(vector<Vehicle>& Vehicles) const;

	Vector align(vector<Vehicle>& Vehicles) const;

	Vector cohesion(vector<Vehicle>& Vehicles) const;

	Vector seek(const Vector& target) const;

	Vector flee(const Vector& target) const;

	void update(const float& dt);

	void apply(const Vector& v) { acc.add(v); }

};

void Vehicle::flock(vector<Vehicle>& Vehicles)
{
	Vector sep = separate(Vehicles);
	Vector ali = align(Vehicles);
	Vector coh = cohesion(Vehicles);
	sep.mul(1.5f);
	ali.mul(1.f);
	coh.mul(1.f);
	apply(sep);
	apply(ali);
	apply(coh);
}

Vector Vehicle::separate(vector<Vehicle>& Vehicles) const
{
	float desiredSeparation = 25.f;
	Vector steer(0, 0);
	int count = 0;
	for (size_t i = 0; i < Vehicles.size(); i++)
	{
		float d = (pos - Vehicles[i].pos).len();
		if ((d > 0) && (d < desiredSeparation))
		{
			Vector diff = pos - Vehicles[i].pos;
			diff.normalize();
			diff.div(d);
			steer.add(diff);
			count++;
		}
	}
	if (count > 0)
		steer.div((float)count);
	if (steer.len() > 0)
	{
		steer.setMag(mSpeed);
		steer.sub(vel);
		steer.limit(mForce);
	}
	return steer;
}

Vector Vehicle::align(vector<Vehicle>& Vehicles) const
{
	float neighborDist = 50.f;
	Vector sum(0, 0);
	int count = 0;
	for (size_t i = 0; i < Vehicles.size(); i++)
	{
		float d = (pos - Vehicles[i].pos).len();
		if ((d > 0) && (d < neighborDist))
		{
			sum.add(Vehicles[i].vel);
			count++;
		}
	}

	if (count > 0)
	{
		sum.setMag(mSpeed);
		Vector steer = sum - vel;
		steer.limit(mForce);
		return steer;
	}
	else
	{
		return Vector(0, 0);
	}

}

Vector Vehicle::cohesion(vector<Vehicle>& Vehicles) const
{
	float neighborDist = 50.f;
	Vector sum(0, 0);
	int count = 0;
	for (size_t i = 0; i < Vehicles.size(); i++)
	{
		float d = (pos - Vehicles[i].pos).len();
		if ((d > 0) && (d < neighborDist))
		{
			sum.add(Vehicles[i].pos);
			count++;
		}
	}

	if (count > 0)
	{
		sum.div((float)count);
		return seek(sum);
	}
	else
	{
		return Vector(0, 0);
	}
}

void Vehicle::update(const float& dt)
{
	vel = vel + acc * dt;
	vel.limit(mSpeed);
	prePos = pos;
	pos = pos + vel * dt;
	if (pos.x < -40)
	{
		pos.x = 1960;
		prePos = pos;
	}
	else if (pos.x > 1960)
	{
		pos.x = -40;
		prePos = pos;
	}
	else if (pos.y < -40)
	{
		pos.y = 1120;
		prePos = pos;
	}
	else if (pos.y > 1120)
	{
		pos.y = -40;
		prePos = pos;
	}
	acc.set(0, 0);
}

Vector Vehicle::seek(const Vector& target) const
{
	Vector force = target - pos;
	force.setMag(mSpeed);
	force.sub(vel);
	force.limit(mForce);
	return force;
}

Vector Vehicle::flee(const Vector& target) const
{
	Vector force = seek(target);
	force.mul(-1.f);
	return force;
}