#pragma once

#include "hittable.h"
#include "vec3.h"

class sphere : public hittable {
public:
    sphere() {}
    sphere(vec3 cen, double r) : cen(cen), r(r) {}

public:
    vec3 center;
    double radius;
};