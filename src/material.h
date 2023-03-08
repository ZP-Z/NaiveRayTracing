#pragma once
#include "defines.h"

struct hit_record;

class material
{
public:
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const = 0;
};

class lambertian : public material
{
public:
	lambertian(const vec3& a) : albedo(a) {}

	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const override
	{
		auto scatter_dir = rec.normal + random_unit_vector();
		// check if direction is zero
		if (scatter_dir.near_zero())
			scatter_dir = rec.normal;
		scattered = ray(rec.p, scatter_dir);
		attenuation = albedo;
		return true;
	}

public:
	vec3 albedo;
};

class metal : public material
{
public:
	metal(const vec3& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered)
		const override
	{
		vec3 reflect_dir = reflect(normalize(r_in.direction()), rec.normal);
		scattered = ray(rec.p, reflect_dir + fuzz * random_in_unit_sphere());
		attenuation = albedo;
		return (dot(scattered.direction(), rec.normal) > 0);
	}

public:
	vec3 albedo; // rays that attenuate
	double fuzz; // fuzzy reflection
};

class dielectric : public material
{
public:
	dielectric(double in_ir) : ir(in_ir) {}

	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered)
		const override
	{
		attenuation = vec3(1.0, 1.0, 1.0);
		double refraction_ratio = rec.front_face ? (1.0 / ir) : ir;

		vec3 ray_dir = normalize(r_in.direction());
		double cos_theta = fmin(dot(-ray_dir, rec.normal), 1.0);
		double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

		bool cannot_refract = refraction_ratio * sin_theta > 1.0;
		vec3 direction;

		if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double())
			direction = reflect(ray_dir, rec.normal);
		else
			direction = refract(ray_dir, rec.normal, refraction_ratio);

		scattered = ray(rec.p, direction);
		return true;
	}

public:
	double ir; // eta / eta'

private:
	static double reflectance(double cosine, double ref_idx)
	{
		// Christophe Schlick's approximation
		auto r0 = (1.0 - ref_idx) / (1.0 + ref_idx);
		r0 = r0 * r0;
		return r0 + (1.0 - r0) * std::pow((1 - cosine), 5);
	}
};