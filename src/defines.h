#pragma once
#include <cmath>
#include <limits>
#include <memory>
#include <random>
#include <cstdlib>

const double BIG_NUMBER = std::numeric_limits<double>::infinity();
const double PI = 3.1415926535897932385;

inline double degree_to_rad(double degree)
{
	return degree * PI / 180.0;
}

inline double random_double()
{
	// Returns a random real in [0,1).
	/*static std::uniform_real_distribution<double> distribution(0.0, 1.0);
	static std::mt19937 generator;
	return distribution(generator);*/

	return rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max)
{
	return min + (max - min) * random_double();
}

inline double clamp(double x, double min, double max)
{
	if (x < min) return min;
	if (x > max) return max;
	return x;
}

#include "ray.h"
#include "vec3.h"