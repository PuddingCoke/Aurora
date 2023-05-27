#pragma once

#ifndef _OBJECTS_H_
#define _OBJECTS_H_

#include<Aurora/EngineAPI/Graphics.h>
#include<Aurora/Utils/Color.h>

#include<iostream>
#include<List>

#include"ShellConfig.h"

class Star;

class Spark;

class Shell;

class Star
{
public:

	static constexpr float drawWidth = 3.0f;

	static constexpr float airDrag = 0.98f;

	static constexpr float airDragHeavy = 0.992f;

	static std::list<Star*>* active;

	static std::list<Star*>* pool;

	static Star* global;

	static Star* add(const float& x, const float& y, const Color& color, const float& angle, const float& speed, const float& life, const float& speedOffX = 0.f, const float& speedOffY = 0.f);

	static void returnInstance(Star* const star);

	bool visible;

	bool heavy;

	float x;

	float y;

	float preX;

	float preY;

	Color color;

	Color secondColor;

	bool haveSecondColor;

	bool colorChanged;

	float speedX;

	float speedY;

	float life;

	float fullLife;

	float transitionTime;

	float spinAngle;

	float spinSpeed;

	float spinRadius;

	float sparkFreq;

	float sparkSpeed;

	float sparkTimer;

	float sparkLife;

	float sparkLifeVariation;

	Color sparkColor;

	bool strobe;

	float strobeFreq;

	Shell* shell;

	void (*onDeath)();

private:

	Star() {}

};

class Spark
{
public:

	static constexpr float airDrag = 0.9f;

	static std::list<Spark*>* active;

	static std::list<Spark*>* pool;

	static Spark* add(const float& x, const float& y, const Color& color, const float& angle, const float& speed, const float& life);

	static void returnInstance(Spark* const spark);

	float x;

	float y;

	float preX;

	float preY;

	Color color;

	float speedX;

	float speedY;

	float life;

private:

	Spark() {}

};


class Shell
{
public:

	Star* comet;

	float shellSize = 3.f;

	float spreadSize = 0.f;

	int starCount = 0;

	float starLife = 0.f;

	float starLifeVariation = 0.f;

	float starDensity = 0.f;

	Glitter glitter;

	Color color;

	Color secondColor;

	Color glitterColor;

	Color pistilColor;

	Color strobeColor;

	bool streamers = false;

	bool crossette = false;

	bool floral = false;

	bool crackle = false;

	bool horseTail = false;

	bool pistil = false;

	bool ring = false;

	bool haveSecondColor = false;

	bool fallingLeaves = false;

	bool strobe = false;

	bool haveStrobeColor = false;

	bool isGhostShell = false;

	static Shell* global;

	static Shell* inner;

	Shell() = delete;

	Shell(const ShellConfiguration& config);

	void launch(const float& launchWidth, const float& launchHeight);

	void burst();

	void burstInner();

};


#endif // !_OBJECTS_H_