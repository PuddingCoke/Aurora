#pragma once

#include<iostream>
#include<Aurora/Math.h>

using namespace std;

class Vector
{
public:

	float x;
	float y;

	Vector() :x(0), y(0) {}

	Vector(float x, float y) :
		x(x), y(y)
	{
	}

	float len() const { return sqrt(this->x * this->x + this->y * this->y); }

	void set(const float& x, const float& y) { this->x = x; this->y = y; }

	void add(const Vector& v) { this->x += v.x; this->y += v.y; }

	void sub(const Vector& v) { this->x -= v.x; this->y -= v.y; }

	void mul(const float& s) { scale(s); }

	void div(const float& s) { scale(1.f / s); }

	void setMag(const float& s) { scale(s / len()); }

	void limit(const float& s) { if (len() > s) setMag(s); }

	void scale(const float& s) { this->x = this->x * s; this->y = this->y * s; }

	void normalize() { setMag(1.f); }

	Vector operator+(const Vector& v) const { return Vector(this->x + v.x, this->y + v.y); }

	Vector operator-(const Vector& v) const { return Vector(this->x - v.x, this->y - v.y); }

	Vector operator*(const float& s) const { return Vector(this->x * s, this->y * s); }

	Vector operator/(const float& s) const { return Vector(this->x / s, this->y / s); }

	const float operator*(const Vector& v) const { return this->x * v.x + this->y * v.y; }

	Vector operator-() { return Vector(-this->x, -this->y); }
};