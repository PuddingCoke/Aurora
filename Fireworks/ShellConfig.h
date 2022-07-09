#pragma once

#ifndef _SHELLCONFIG_H_
#define _SHELLCONFIG_H_

#include<Aurora/Color.h>
#include<Aurora/Math.h>
#include<Aurora/Random.h>

enum class Glitter
{
	none, light, medium, heavy, thick, streamer, willow
};

struct ShellConfiguration
{
	float shellSize = 0.f;

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

	bool isGhostShell = false;

	bool strobe = false;

	bool haveStrobeColor = false;

	static float globalShellSize;

};

enum class ShellType
{
	random,
	crysanthemumShell,
	ghostShell,
	strobeShell,
	palmShell,
	ringShell,
	crossetteShell,
	floralShell,
	fallingLeavesShell,
	willowShell,
	crackleShell,
	horsetailShell
};

ShellConfiguration crysanthemumShell();

ShellConfiguration ghostShell();

ShellConfiguration strobeShell();

ShellConfiguration palmShell();

ShellConfiguration ringShell();

ShellConfiguration crossetteShell();

ShellConfiguration floralShell();

ShellConfiguration fallingLeavesShell();

ShellConfiguration willowShell();

ShellConfiguration crackleShell();

ShellConfiguration horsetailShell();

ShellConfiguration getShellConfig(ShellType type);

#endif // !_SHELLCONFIG_H_
