#include<Aurora/Random.h>

std::mt19937 Random::randomEngine = std::mt19937((unsigned int)time(nullptr));

std::normal_distribution<float> Random::normalDistribution = std::normal_distribution<float>(0.f, 1.f);

std::uniform_real_distribution<float> Random::floatDistribution = std::uniform_real_distribution<float>(0.f, 1.f);

std::uniform_real_distribution<double> Random::doubleDistribution = std::uniform_real_distribution<double>(0.f, 1.f);

std::uniform_int_distribution<int> Random::intDistribution;

std::uniform_int_distribution<unsigned int> Random::uintDistribution;

float Random::Gauss()
{
	return normalDistribution(randomEngine);
}

float Random::Float()
{
	return floatDistribution(randomEngine);
}

double Random::Double()
{
	return doubleDistribution(randomEngine);
}

int Random::Int()
{
	return intDistribution(randomEngine);
}

unsigned int Random::Uint()
{
	return uintDistribution(randomEngine);
}
