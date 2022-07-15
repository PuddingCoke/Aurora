#include"ShellConfig.h"

float ShellConfiguration::globalShellSize = 2.f;

Color whiteOrGold()
{
	return Random::Float() < 0.5f ? Color::Gold : Color::White;
}

Color makeDifColor(const Color& color)
{
	Color outColor = Color::random();
	while (outColor == color)
	{
		outColor = Color::random();
	}
	return outColor;
}

Color makePistilColor(const Color& shellColor)
{
	if (shellColor == Color::White || shellColor == Color::Gold)
	{
		return makeDifColor(shellColor);
	}
	else
	{
		return whiteOrGold();
	}
}

ShellConfiguration crysanthemumShell()
{
	ShellConfiguration config;

	const bool glitter = Random::Float() < 0.25f;

	const bool singleColor = Random::Float() < 0.72f;

	const bool pistil = singleColor && Random::Float() < 0.42f;

	config.color = Color::random();

	config.shellSize = ShellConfiguration::globalShellSize;

	config.spreadSize = 300.f + ShellConfiguration::globalShellSize * 100.f;

	config.starLife = 0.9f + config.shellSize * 0.2f;

	config.starDensity = glitter ? 1.1f : 1.25f;

	config.glitter = glitter ? Glitter::light : Glitter::none;

	config.glitterColor = whiteOrGold();

	config.pistil = pistil;

	if (pistil)
	{
		config.pistilColor = makePistilColor(config.color);
	}

	if (!singleColor)
	{
		config.haveSecondColor = true;
		if (pistil)
		{
			config.secondColor = config.pistilColor;
		}
		else
		{
			config.secondColor = makeDifColor(config.color);
		}
	}

	config.streamers = !pistil && config.color != Color::White && Random::Float() < 0.42f;

	return config;
}

ShellConfiguration ghostShell()
{
	ShellConfiguration config = crysanthemumShell();

	config.starLife *= 1.5f;

	const Color ghostColor = makeDifColor(Color::White);

	config.streamers = true;

	config.pistil = Random::Float() < 0.42f;

	if (config.pistil)
	{
		config.pistilColor = makePistilColor(ghostColor);
	}

	config.color = Color::Invisible;

	config.haveSecondColor = true;

	config.secondColor = ghostColor;

	config.glitter = Glitter::none;

	config.isGhostShell = true;

	return config;
}

ShellConfiguration strobeShell()
{
	ShellConfiguration config;

	config.shellSize = ShellConfiguration::globalShellSize;

	config.spreadSize = 280.f + ShellConfiguration::globalShellSize * 92.f;

	config.starLife = 1.1f + ShellConfiguration::globalShellSize * 0.2f;

	config.starLifeVariation = 0.4f;

	config.starDensity = 1.1f;

	config.color = Color::random();

	config.glitter = Glitter::light;

	config.glitterColor = Color::White;

	config.strobe = true;

	if (Random::Float() < 0.5f)
	{
		config.haveStrobeColor = true;
		config.strobeColor = Color::White;
	}
	else
	{
		config.haveStrobeColor = false;
	}

	config.pistil = Random::Float() < 0.5f;

	if (config.pistil)
	{
		config.pistilColor = makePistilColor(config.color);
	}

	return config;
}

ShellConfiguration palmShell()
{
	const bool thick = Random::Float() < 0.5f;

	ShellConfiguration config;

	config.shellSize = ShellConfiguration::globalShellSize;

	config.color = Color::random();

	config.spreadSize = 250.f + ShellConfiguration::globalShellSize * 75.f;

	config.starDensity = thick ? 0.15f : 0.4f;

	config.starLife = 1.8f + ShellConfiguration::globalShellSize * 0.2f;

	config.glitter = thick ? Glitter::thick : Glitter::heavy;

	return config;
}

ShellConfiguration ringShell()
{
	const bool pistil = Random::Float() < 0.75f;

	ShellConfiguration config;

	config.shellSize = ShellConfiguration::globalShellSize;

	config.ring = true;

	config.color = Color::random();

	config.spreadSize = 300.f + ShellConfiguration::globalShellSize * 100.f;

	config.starLife = 0.9f + ShellConfiguration::globalShellSize * 0.2f;

	config.starCount = 2.2f * Math::two_pi * (ShellConfiguration::globalShellSize + 1);

	config.pistil = pistil;

	if (pistil)
	{
		config.pistilColor = makePistilColor(config.color);
	}

	config.glitter = !pistil ? Glitter::light : Glitter::none;

	config.glitterColor = config.color == Color::Gold ? Color::Gold : Color::White;

	config.streamers = Random::Float() < 0.3f;

	return config;
}

ShellConfiguration crossetteShell()
{
	ShellConfiguration config;

	config.shellSize = ShellConfiguration::globalShellSize;

	config.spreadSize = 300.f + ShellConfiguration::globalShellSize * 100.f;

	config.starLife = 0.75f + ShellConfiguration::globalShellSize * 0.16f;

	config.starLifeVariation = 0.4f;

	config.starDensity = 0.85f;

	config.color = Color::random();

	config.crossette = true;

	config.pistil = Random::Float() < 0.5f;

	if (config.pistil)
	{
		config.pistilColor = makePistilColor(config.color);
	}

	return config;
}

ShellConfiguration floralShell()
{
	ShellConfiguration config;

	config.shellSize = ShellConfiguration::globalShellSize;

	config.spreadSize = 300.f + ShellConfiguration::globalShellSize * 120.f;

	config.starDensity = 0.12f;

	config.starLife = 0.5f + ShellConfiguration::globalShellSize * 0.05f;

	config.starLifeVariation = 0.5f;

	config.color = Color::random();

	config.floral = true;

	return config;
}


ShellConfiguration fallingLeavesShell()
{
	ShellConfiguration config;

	config.shellSize = ShellConfiguration::globalShellSize;

	config.color = Color::Invisible;

	config.spreadSize = 300.f + ShellConfiguration::globalShellSize * 120.f;

	config.starDensity = 0.12f;

	config.starLife = 0.5f + ShellConfiguration::globalShellSize * 0.05f;

	config.starLifeVariation = 0.5f;

	config.glitter = Glitter::medium;

	config.glitterColor = Color::Gold;

	config.fallingLeaves = true;

	return config;
}

ShellConfiguration willowShell()
{
	ShellConfiguration config;

	config.shellSize = ShellConfiguration::globalShellSize;

	config.spreadSize = 300.f + ShellConfiguration::globalShellSize * 100.f;

	config.starDensity = 0.6f;

	config.starLife = 3.f + ShellConfiguration::globalShellSize * 0.3f;

	config.glitter = Glitter::willow;

	config.glitterColor = Color::Gold;

	config.color = Color::Invisible;

	return config;
}

ShellConfiguration crackleShell()
{
	ShellConfiguration config;

	config.shellSize = ShellConfiguration::globalShellSize;

	config.spreadSize = 380.f + ShellConfiguration::globalShellSize * 75.f;

	config.starDensity = 1.f;

	config.starLife = 0.6f + ShellConfiguration::globalShellSize * 0.1f;

	config.starLifeVariation = 0.32f;

	config.glitter = Glitter::light;

	config.glitterColor = Color::Gold;

	config.color = Random::Float() < 0.75f ? Color::Gold : Color::random();

	config.crackle = true;

	config.pistil = Random::Float() < 0.65f;

	if (config.pistil)
	{
		config.pistilColor = makePistilColor(config.color);
	}

	return config;
}

ShellConfiguration horsetailShell()
{
	ShellConfiguration config;

	config.shellSize = ShellConfiguration::globalShellSize;

	config.horseTail = true;

	config.color = Color::random();

	config.spreadSize = 250.f + ShellConfiguration::globalShellSize * 38.f;

	config.starDensity = 0.9f;

	config.starLife = 2.5f + ShellConfiguration::globalShellSize * 0.3f;

	config.glitter = Glitter::medium;
	
	config.glitterColor = Random::Float() < 0.5f ? whiteOrGold() : config.color;

	if (config.color == Color::White)
	{
		config.strobe = true;
	}

	return config;
}

ShellConfiguration getShellConfig(ShellType type)
{

	if (type == ShellType::random)
	{
		type = (ShellType)(rand() % 11 + 1);
	}

	switch (type)
	{
	case ShellType::crysanthemumShell:
		return crysanthemumShell();
	case ShellType::ghostShell:
		return ghostShell();
	case ShellType::strobeShell:
		return strobeShell();
	case ShellType::palmShell:
		return palmShell();
	case ShellType::ringShell:
		return ringShell();
	case ShellType::crossetteShell:
		return crossetteShell();
	case ShellType::floralShell:
		return floralShell();
	case ShellType::fallingLeavesShell:
		return fallingLeavesShell();
	case ShellType::willowShell:
		return willowShell();
	case ShellType::crackleShell:
		return crackleShell();
	case ShellType::horsetailShell:
		return horsetailShell();
	default:
		return strobeShell();
	}
}
