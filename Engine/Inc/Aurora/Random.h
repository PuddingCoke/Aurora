#pragma once

#ifndef _RANDOM_H_
#define _RANDOM_H_

#include<random>
#include<cstdlib>
#include<ctime>

class Random
{
public:

	Random() = delete;

	Random(const Random&) = delete;

	void operator=(const Random&) = delete;

	static float Float();

	//mean 0 sd(sigma) 1
	static float Gauss();

	static double Double();

	static int Int();

private:

	static std::mt19937 randomEngine;

	static std::uniform_int_distribution<int> intDistribution;

	static std::normal_distribution<float> normalDistribution;
};

#endif // !_RANDOM_H_
