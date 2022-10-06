#include<Aurora/Random.h>

std::mt19937 Random::randomEngine = std::mt19937((unsigned int)time(nullptr));

std::uniform_int_distribution<int> Random::intDistribution;

std::normal_distribution<float> Random::normalDistribution = std::normal_distribution<float>(0.f, 1.f);

float Random::Float()
{
	return intDistribution(randomEngine) / (float)std::numeric_limits<int>::max();
}

float Random::Gauss()
{
	return normalDistribution(randomEngine);
}

double Random::Double()
{
	return intDistribution(randomEngine) / (double)std::numeric_limits<int>::max();
}

int Random::Int()
{
	return intDistribution(randomEngine);
}
