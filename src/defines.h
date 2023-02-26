#pragma once
#include <cmath>
#include <limits>
#include <memory>

const double BIG_NUMBER = std::numeric_limits<double>::infinity();
const double PI = 3.1415926535897932385;

inline double degree_to_rad(double degree)
{
	return degree * PI / 180.0;
}

#include "ray.h"
#include "vec3.h"