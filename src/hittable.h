#pragma once
#include "ray.h"
#include "defines.h"

class material;

struct hit_record {
public:
    vec3 p;
    vec3 normal;
	std::shared_ptr<material> mat;
    double t;
    bool front_face;

	// check if normal and ray's direction is the same, prevent the back of a geometry
    inline void set_face_normal(const ray& r, const vec3& outward_normal)
    {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable {
public:
    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;
};