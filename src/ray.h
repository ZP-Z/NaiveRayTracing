#pragma once
#include "vec3.h"

class ray
{
public:
	ray() {}
	ray(const vec3& origin, const vec3& direction)
		: ori(origin), dir(direction)
	{}

	vec3 origin() const { return ori; }
	vec3 direction() const { return dir; }

	vec3 at(double t) const
	{
		return ori + t * dir;
	}

public:
	vec3 ori;
	vec3 dir;
};