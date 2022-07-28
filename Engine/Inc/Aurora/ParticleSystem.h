#pragma once

#ifndef _PARTICLE_SYSTEM_H_
#define _PARTICLE_SYSTEM_H_

#include"Shader.h"
#include"Random.h"
#include"Math.h"

class ParticleSystem
{
public:

	ParticleSystem()
	{

	}

	virtual ~ParticleSystem()
	{

	}

	virtual void compileShaders() = 0;

	virtual void update(const float& dt) = 0;

	virtual void render() = 0;

};

#endif // !_PARTICLE_SYSTEM_H_
