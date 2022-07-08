#include<Aurora/Random.hpp>

std::mt19937 Random::randomEngine;

std::uniform_int_distribution<int> Random::intDistribution;

float Random::Float()
{
	return intDistribution(randomEngine) / (float)std::numeric_limits<int>::max();
}

double Random::Double()
{
	return intDistribution(randomEngine) / (double)std::numeric_limits<int>::max();
}

int Random::Int()
{
	return intDistribution(randomEngine);
}

void Random::ini()
{
	randomEngine.seed((unsigned int)time(nullptr));
}
