﻿#pragma once

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

	static float Float();

	static double Double();

	static int Int();

private:

	friend class Aurora;

	static std::mt19937 randomEngine;

	static std::uniform_int_distribution<int> intDistribution;

};

#endif // !_RANDOM_H_
