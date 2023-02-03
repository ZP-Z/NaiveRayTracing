#include "color.h"
#include "ray.h"
#include "vec3.h"

#include <iostream>

double hit_sphere(const vec3& center, double radius, const ray& r)
{
	/** (Ray(t) - Center).length() = radius, solve t.
	*	(ori + t * dir - center) * (ori + t * dir - center) = r^2
	*	so we get: t^2 * dir * dir + 2 * t * dir * (ori - center) + (ori - center) * (ori - center) - r^2 = 0
	*	its a quadratic equation ax^2 + bx + c = 0
	*/
	vec3 oc = r.origin() - center;
	auto a = dot(r.direction(), r.direction());
	auto b = 2.0 * dot(oc, r.direction());
	auto c = dot(oc, oc) - radius * radius;

	auto discriminant = b * b - 4 * a * c;
	if (discriminant < 0)
		return -1.0;
	else
		return (-b - std::sqrt(discriminant)) / (2.0 * a);
}

double hit_sphere_simple(const vec3& center, double radius, const ray& r)
{
	/** (Ray(t) - Center).length() = radius, solve t.
	*	(ori + t * dir - center) * (ori + t * dir - center) = r^2
	*	so we get: t^2 * dir * dir + 2 * t * dir * (ori - center) + (ori - center) * (ori - center) - r^2 = 0
	*	its a quadratic equation ax^2 + bx + c = 0
	*/
	vec3 oc = r.origin() - center;
	auto a = r.direction().length_squared();
	auto half_b = dot(oc, r.direction());
	auto c = oc.length_squared() - radius * radius;
	auto discriminant = half_b * half_b - a * c;

	if (discriminant < 0)
		return -1.0;
	else
		return (-half_b - std::sqrt(discriminant)) / a;
}

vec3 ray_color(const ray& r)
{
	// when hit the sphere
	auto t = hit_sphere(vec3(0, 0, -1), 0.5, r);
	if (t > 0)
	{
		vec3 N = normalize(r.at(t) - vec3(0, 0, -1));
		return 0.5 * vec3(N.x() + 1, N.y() + 1, N.z() + 1);
	}

	// blue background
	vec3 unit_directuion = normalize(r.direction());
	t = 0.5 * (unit_directuion.y() + 1.0);
	return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
}

int main()
{
    // Image
	const auto aspect_ratio = 16.0 / 9.0;
    const int image_width = 400;
	const int image_height = static_cast<int>(image_width / aspect_ratio);

	// Camera
	auto viewport_height = 2.0;
	auto viewport_width = aspect_ratio * viewport_height;
	auto focal_length = 1.0;

	auto origin = vec3(0, 0, 0);
	auto horizontal = vec3(viewport_width, 0, 0);
	auto vertical = vec3(0, viewport_height, 0);
	auto lower_left_corner = origin - horizontal / 2 - vertical / 2 - vec3(0, 0, focal_length);

    // Render
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

	for (int j = image_height - 1; j >= 0; --j) {
		std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
		for (int i = 0; i < image_width; ++i) {

			auto u = double(i) / (image_width - 1);
			auto v = double(j) / (image_height - 1);
			ray r(origin, lower_left_corner + u * horizontal + v * vertical - origin);
			vec3 pixel_color = ray_color(r);

			//vec3 pixel_color(double(i) / (image_width - 1), double(j) / (image_height - 1), 0.25);
			write_color(std::cout, pixel_color);
		}
	}
	std::cerr << "\nDone.\n";

    return 0;
}