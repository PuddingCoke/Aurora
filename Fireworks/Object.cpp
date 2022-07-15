#include"Objects.h"

Star* Star::global;

Shell* Shell::global;

Shell* Shell::inner;

std::list<Star*>* Star::active = nullptr;

std::list<Star*>* Star::pool = nullptr;

std::list<Spark*>* Spark::active = nullptr;

std::list<Spark*>* Spark::pool = nullptr;

float global_sparkFreq;
float global_sparkSpeed;
float global_sparkLife;
float global_sparkLifeVariation;

float global_ringStartAngle;
float global_ringSquash;

float global_x;
float global_y;

Color randomColor;

void createParticleArc(const float& start, const float& arcLength, const int& count, const float& randomness, void (*particleFactory)(const float&))
{
	const float angleDelta = arcLength / count;

	const float end = start + arcLength - (angleDelta * 0.5f);

	if (end > start)
	{
		for (float angle = start; angle < end; angle += angleDelta)
		{
			particleFactory(angle + Random::Float() * angleDelta * randomness);
		}
	}
	else
	{
		for (float angle = start; angle > end; angle += angleDelta)
		{
			particleFactory(angle + Random::Float() * angleDelta * randomness);
		}
	}
}

void createBurst(const int& count, void (*particleFactory)(const float&, const float&), const float& startAngle = 0, const float arcLength = Math::two_pi)
{
	const float R = 0.5f * sqrtf(count / Math::pi);

	const float C = 2.f * R * Math::pi;

	const float C_HALF = C / 2.f;

	for (int i = 0; i <= C_HALF; i++)
	{
		const float ringAngle = i / C_HALF * Math::half_pi;
		const float ringSize = cosf(ringAngle);
		const float partsPerFullRing = C * ringSize;
		const float partsPerArc = partsPerFullRing * (arcLength / Math::two_pi);

		const float angleInc = Math::two_pi / partsPerFullRing;
		const float angleOffset = Random::Float() * angleInc + startAngle;
		const float maxRandomAngleOffset = angleInc * 0.33f;

		for (int j = 0; j < partsPerArc; j++)
		{
			const float randomAngleOffset = Random::Float() * maxRandomAngleOffset;
			const float angle = angleInc * j + angleOffset + randomAngleOffset;
			particleFactory(angle, ringSize);
		}
	}
}

void crossetteEffect()
{
	const float startAngle = Random::Float() * Math::half_pi;
	auto particleFactory = [](const float& angle)
	{
		Star::add(
			Star::global->x,
			Star::global->y,
			Star::global->color,
			angle,
			Random::Float() * 0.6f + 0.75f,
			0.6f
		);
	};
	createParticleArc(startAngle, Math::two_pi, 4, 0.5f, particleFactory);
}

void floralEffect()
{
	randomColor = Color::random();

	const int count = 30;
	auto particleFactory = [](const float& angle, const float& speedMult)
	{
		Star* const star = Star::add(
			Star::global->x,
			Star::global->y,
			randomColor,
			angle,
			speedMult * 2.4f,
			1.f + Random::Float() * .3f,
			Star::global->speedX,
			Star::global->speedY
		);
	};
	createBurst(count, particleFactory);
}

void fallingLeavesEffect()
{
	auto particleFactory = [](const float& angle, const float& speedMult)
	{
		Star* const star = Star::add(
			Star::global->x,
			Star::global->y,
			Color::Invisible,
			angle,
			speedMult * 2.4f,
			2.4f + Random::Float() * 0.6f,
			Star::global->speedX,
			Star::global->speedY
		);
		star->sparkColor = Color::Gold;
		star->sparkFreq = 0.048f;
		star->sparkSpeed = 0.28f;
		star->sparkLife = 0.75f;
		star->sparkLifeVariation = 3.2f;
	};
	createBurst(7, particleFactory);
}

void crackleEffect()
{
	const int count = 32;
	auto particleFactory = [](const float& angle)
	{
		Spark::add(
			Star::global->x,
			Star::global->y,
			Color::Gold,
			angle,
			powf(Random::Float(), 0.45f) * 2.4f,
			0.3f + Random::Float() * 0.2f
		);
	};
	createParticleArc(0, Math::two_pi, count, 1.8f, particleFactory);
}

Star* Star::add(const float& x, const float& y, const Color& color, const float& angle, const float& speed, const float& life, const float& speedOffX, const float& speedOffY)
{
	Star* instance = nullptr;

	if (!pool->empty())
	{
		instance = pool->back();
		pool->pop_back();
	}
	else
	{
		instance = new Star();
	}

	instance->visible = true;
	instance->heavy = false;
	instance->x = x;
	instance->y = y;
	instance->preX = x;
	instance->preY = y;
	instance->color = color;
	instance->speedX = sinf(angle) * speed + speedOffX;
	instance->speedY = cosf(angle) * speed + speedOffY;
	instance->life = life;
	instance->fullLife = life;
	instance->spinAngle = Random::Float() * Math::two_pi;
	instance->spinSpeed = 0.8f;
	instance->spinRadius = 0;
	instance->sparkFreq = 0;
	instance->sparkSpeed = 1;
	instance->sparkTimer = 0;
	instance->sparkColor = color;
	instance->sparkLife = 0.75f;
	instance->sparkLifeVariation = 0.25f;
	instance->strobe = false;
	instance->haveSecondColor = false;
	instance->transitionTime = 0;
	instance->colorChanged = false;
	instance->strobeFreq = 0;

	instance->onDeath = nullptr;
	instance->shell = nullptr;

	active->push_back(instance);
	return instance;
}

void Star::returnInstance(Star* const star)
{
	global = star;
	if (star->onDeath)
	{
		star->onDeath();
	}
	else if (star->shell)
	{
		Shell::global = star->shell;
		star->shell->burst();
		delete star->shell;
		star->shell = nullptr;
	}
	pool->push_back(star);
}

Spark* Spark::add(const float& x, const float& y, const Color& color, const float& angle, const float& speed, const float& life)
{
	Spark* instance;

	if (!pool->empty())
	{
		instance = pool->back();
		pool->pop_back();
	}
	else
	{
		instance = new Spark();
	}

	instance->x = x;
	instance->y = y;
	instance->preX = x;
	instance->preY = y;
	instance->color = color;
	instance->speedX = sinf(angle) * speed;
	instance->speedY = cosf(angle) * speed;
	instance->life = life;

	active->push_back(instance);
	return instance;
}

void Spark::returnInstance(Spark* const spark)
{
	pool->push_back(spark);
}

Shell::Shell(const ShellConfiguration& config) :
	comet(nullptr)
{
	shellSize = config.shellSize;

	spreadSize = config.spreadSize;

	starCount = config.starCount;

	starLife = config.starLife;

	starLifeVariation = config.starLifeVariation;

	starDensity = config.starDensity;

	glitter = config.glitter;

	color = config.color;

	secondColor = config.secondColor;

	glitterColor = config.glitterColor;

	pistilColor = config.pistilColor;

	strobeColor = config.strobeColor;

	streamers = config.streamers;

	crossette = config.crossette;

	floral = config.floral;

	crackle = config.crackle;

	horseTail = config.horseTail;

	pistil = config.pistil;

	ring = config.ring;

	haveSecondColor = config.haveSecondColor;

	fallingLeaves = config.fallingLeaves;

	strobe = config.strobe;

	haveStrobeColor = config.haveStrobeColor;

	starLifeVariation = config.starLifeVariation || 0.125f;

	isGhostShell = config.isGhostShell;

	if (!starCount)
	{
		const float density = config.starDensity || 1;
		const float scaledSize = spreadSize / 54.f;
		starCount = (int)fmaxf(6, scaledSize * scaledSize * density);
	}
}

void Shell::launch(const float& launchWidth, const float& launchHeight)
{
	const float launchVelocity = powf(launchHeight * 0.04f, 0.64f);

	Color targetColor;

	if (isGhostShell)
	{
		targetColor = Color::Invisible;
	}
	else
	{
		targetColor = Color::random();
	}

	Star* const comet = Star::add(
		launchWidth,
		0,
		targetColor,
		0,
		launchVelocity * (horseTail ? 1.2f : 1.0f),
		launchVelocity * (horseTail ? 0.1f : 0.4f)
	);

	this->comet = comet;
	this->comet->shell = this;

	comet->heavy = true;
	comet->spinRadius = Random::Float() * (0.85f - 0.32f) + 0.32f;
	comet->sparkFreq = 0.008f;
	comet->sparkLife = 0.32f;
	comet->sparkLifeVariation = 3.f;

	if (glitter == Glitter::willow || fallingLeaves)
	{
		comet->sparkFreq = 0.006f;
		comet->sparkSpeed = 0.5f;
		comet->sparkLife = 0.5f;
	}

	if (color == Color::Invisible)
	{
		comet->sparkColor = Color::Gold;
	}

	if (Random::Float() > 0.4f && !horseTail)
	{
		comet->haveSecondColor = true;
		comet->secondColor = Color::Invisible;
		comet->transitionTime = powf(Random::Float(), 1.5f) * 0.7f + 0.5f;
	}

}

void Shell::burst()
{
	switch (Shell::global->glitter)
	{
	case Glitter::light:
		global_sparkFreq = 0.4f;
		global_sparkSpeed = 0.3f;
		global_sparkLife = 0.3f;
		global_sparkLifeVariation = 2.f;
		break;
	case Glitter::medium:
		global_sparkFreq = 0.2f;
		global_sparkSpeed = 0.44f;
		global_sparkLife = 0.7f;
		global_sparkLifeVariation = 2.f;
		break;
	case Glitter::heavy:
		global_sparkFreq = 0.08f;
		global_sparkSpeed = 0.8f;
		global_sparkLife = 1.4f;
		global_sparkLifeVariation = 2.f;
		break;
	case Glitter::thick:
		global_sparkFreq = 0.016f;
		global_sparkSpeed = 1.65f;
		global_sparkLife = 1.4f;
		global_sparkLifeVariation = 3.f;
		break;
	case Glitter::streamer:
		global_sparkFreq = 0.032f;
		global_sparkSpeed = 1.05f;
		global_sparkLife = 0.62f;
		global_sparkLifeVariation = 2.f;
		break;
	case Glitter::willow:
		global_sparkFreq = 0.12f;
		global_sparkSpeed = 0.34f;
		global_sparkLife = 1.4f;
		global_sparkLifeVariation = 3.8f;
		break;
	default:
		break;
	}

	global_sparkFreq /= 3.f;

	auto particleFactory = [](const float& anlge, const float& speedMult)
	{
		const float speed = Shell::global->spreadSize / 96;

		const float standardInitialSpeed = Shell::global->spreadSize / 1800.f;

		float local_speedOffX;

		float local_speedOffY;

		if (Shell::global->horseTail && Shell::global->comet)
		{
			local_speedOffX = Shell::global->comet->speedX;
			local_speedOffY = Shell::global->comet->speedY;
		}
		else
		{
			local_speedOffX = 0;
			local_speedOffY = standardInitialSpeed;
		}

		Star* star = Star::add(
			Star::global->x,
			Star::global->y,
			Shell::global->color,
			anlge,
			speedMult * speed,
			Shell::global->starLife + Random::Float() * Shell::global->starLife * Shell::global->starLifeVariation,
			local_speedOffX,
			local_speedOffY
		);

		if (Shell::global->haveSecondColor)
		{
			star->transitionTime = Shell::global->starLife * (Random::Float() * 0.05f + 0.32f);
			star->secondColor = Shell::global->secondColor;
		}

		if (Shell::global->strobe)
		{
			star->transitionTime = Shell::global->starLife * (Random::Float() * 0.08f + 0.46f);
			star->strobe = true;
			star->strobeFreq = Random::Float() * 0.02f + 0.04f;
			if (Shell::global->haveStrobeColor)
			{
				star->secondColor = Shell::global->strobeColor;
			}
		}

		if (Shell::global->crossette)
		{
			star->onDeath = crossetteEffect;
		}
		else if (Shell::global->crackle)
		{
			star->onDeath = crackleEffect;
		}
		else if (Shell::global->floral)
		{
			star->onDeath = floralEffect;
		}
		else if (Shell::global->fallingLeaves)
		{
			star->onDeath = fallingLeavesEffect;
		}

		if (Shell::global->glitter != Glitter::none)
		{
			star->sparkFreq = global_sparkFreq;
			star->sparkSpeed = global_sparkSpeed;
			star->sparkLife = global_sparkLife;
			star->sparkLifeVariation = global_sparkLifeVariation;
			star->sparkColor = Shell::global->glitterColor;
			star->sparkTimer = Random::Float() * star->sparkFreq;
		}

	};

	if (Shell::global->ring)
	{
		global_ringStartAngle = Random::Float() * Math::pi;
		global_ringSquash = powf(Random::Float(), 2) * 0.85f + 0.15f;

		auto particleFactory = [](const float& angle)
		{
			const float speed = Shell::global->spreadSize / 96;
			const float initSpeedX = sinf(angle) * speed * global_ringSquash;
			const float initSpeedY = cosf(angle) * speed;
			const float newSpeed = sqrtf(initSpeedX * initSpeedX + initSpeedY * initSpeedY);
			const float newAngle = atan2f(initSpeedY, initSpeedX) + Math::half_pi + global_ringStartAngle;
			Star* const star = Star::add(
				Star::global->x,
				Star::global->y,
				Shell::global->color,
				newAngle,
				newSpeed,
				Shell::global->starLife + Random::Float() * Shell::global->starLife * Shell::global->starLifeVariation
			);

			if (Shell::global->glitter != Glitter::none)
			{
				star->sparkFreq = global_sparkFreq;
				star->sparkSpeed = global_sparkSpeed;
				star->sparkLife = global_sparkLife;
				star->sparkLifeVariation = global_sparkLifeVariation;
				star->sparkColor = Shell::global->glitterColor;
				star->sparkTimer = Random::Float() * star->sparkFreq;
			}
		};
		createParticleArc(0, Math::two_pi, starCount, 0, particleFactory);
	}
	else
	{
		createBurst(starCount, particleFactory);
	}

	if (pistil)
	{
		ShellConfiguration innerConfig;
		innerConfig.spreadSize = spreadSize * 0.5f;
		innerConfig.starLife = starLife * 0.6f;
		innerConfig.starLifeVariation = starLifeVariation;
		innerConfig.starDensity = 1.4f;
		innerConfig.color = pistilColor;
		innerConfig.glitter = Glitter::light;
		innerConfig.glitterColor = pistilColor == Color::Gold ? Color::Gold : Color::White;
		Shell* innerShell = new Shell(innerConfig);
		Shell::inner = innerShell;
		global_x = comet->x;
		global_y = comet->y;
		innerShell->burstInner();
		delete innerShell;
		Shell::inner = nullptr;
	}
	if (streamers)
	{
		ShellConfiguration innerConfig;
		innerConfig.spreadSize = spreadSize * 0.9f;
		innerConfig.starLife = starLife * 0.8f;
		innerConfig.starLifeVariation = starLifeVariation;
		innerConfig.starCount = floorf(fmaxf(6, spreadSize / 45.f));
		innerConfig.color = Color::White;
		innerConfig.glitter = Glitter::streamer;
		Shell* innerShell = new Shell(innerConfig);
		Shell::inner = innerShell;
		global_x = comet->x;
		global_y = comet->y;
		innerShell->burstInner();
		delete innerShell;
		Shell::inner = nullptr;
	}

}

void Shell::burstInner()
{
	switch (Shell::inner->glitter)
	{
	case Glitter::light:
		global_sparkFreq = 0.4f;
		global_sparkSpeed = 0.3f;
		global_sparkLife = 0.3f;
		global_sparkLifeVariation = 2.f;
		break;
	case Glitter::streamer:
		global_sparkFreq = 0.032f;
		global_sparkSpeed = 1.05f;
		global_sparkLife = 0.62f;
		global_sparkLifeVariation = 2.f;
		break;
	default:
		break;
	}

	global_sparkFreq /= 3.f;

	auto particleFactory = [](const float& anlge, const float& speedMult)
	{
		const float speed = Shell::inner->spreadSize / 96;

		const float standardInitialSpeed = Shell::inner->spreadSize / 1800.f;

		Star* star = Star::add(
			global_x,
			global_y,
			Shell::inner->color,
			anlge,
			speedMult * speed,
			Shell::inner->starLife + Random::Float() * Shell::inner->starLife * Shell::inner->starLifeVariation,
			0,
			standardInitialSpeed
		);

		star->sparkFreq = global_sparkFreq;
		star->sparkSpeed = global_sparkSpeed;
		star->sparkLife = global_sparkLife;
		star->sparkLifeVariation = global_sparkLifeVariation;
		star->sparkColor = Shell::inner->glitterColor;
		star->sparkTimer = Random::Float() * star->sparkFreq;

	};

	createBurst(starCount, particleFactory);
}