#pragma once
#include "hittable.h"

#include <memory.h>
#include <vector>

class hittble_list : public hittable {
public:
	hittble_list() {}
	hittble_list(std::shared_ptr<hittable> object){ add(object); }

	void clear() { objects.clear(); }
	void add(std::shared_ptr<hittable> object) { objects.push_back(object); }

	// check our objects and get the closest object the ray hit
	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;

public:
	std::vector<std::shared_ptr<hittable>> objects;
};

bool hittble_list::hit(const ray& r, double t_min, double t_max, hit_record& rec) const
{
	hit_record temp_rec;
	bool is_hit = false;
	auto closest_t = t_max;

	for (const auto& object : objects)
	{
		if (object->hit(r, t_min, closest_t, temp_rec))
		{
			is_hit = true;
			closest_t = temp_rec.t;
			rec = temp_rec;
		}
	}

	return is_hit;
}
