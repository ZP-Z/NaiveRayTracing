#include "defines.h"

#include "color.h"
#include "hittble_list.h"
#include "sphere.h"
#include "camera.h"

#include <iostream>
#include <fstream>

// ray tracing for the objects in the world
vec3 ray_color(const ray& r, const hittable& world, int depth)
{
	hit_record rec;

	// if exceeded the ray bounce, no light
	if (depth <= 0)
		return vec3(0, 0, 0);

	// when hit the object
	if (world.hit(r, 0.001, BIG_NUMBER, rec))
	{
		//return 0.5 * (rec.normal + vec3(1, 1, 1));
		// do ray tracing with a hack random ray direction
		//vec3 target = rec.p + rec.normal + random_in_unit_sphere();
		//vec3 target = rec.p + rec.normal + random_unit_vector();
		vec3 target = rec.p + rec.normal + random_in_hemisphere(rec.normal);
		return 0.5 * ray_color(ray(rec.p, target - rec.p), world, depth - 1);
	}

	// blue background
	vec3 unit_direction = normalize(r.direction());
	auto t = 0.5 * (unit_direction.y() + 1.0);
	return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
}

int main(int argc, char* argv)
{
    // Image
	const auto aspect_ratio = 16.0 / 9.0;
    const int image_width = 400;
	const int image_height = static_cast<int>(image_width / aspect_ratio);
	const int samples_per_pixel = 100;
	const int max_depth = 50;

	// World
	hittble_list world;
	world.add(std::make_shared<sphere>(vec3(0, 0, -1), 0.5));
	world.add(std::make_shared<sphere>(vec3(0, -100.5, -1), 100));

	// Camera
	camera cam;

	static bool use_antialiasing = true;

    // Render
	if (argc > 1)
	{
		std::ofstream out(argv);
		if (!out)
		{
			std::cout << "Could not open file " << argv << std::endl;
			return -1;
		}

		out << "P3\n" << image_width << ' ' << image_height << "\n255\n";

		for (int j = image_height - 1; j >= 0; --j) {
			std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
			for (int i = 0; i < image_width; ++i) {
				if (use_antialiasing)
				{
					vec3 pixel_color(0, 0, 0);
					for (int s = 0; s < samples_per_pixel; ++s) {
						auto u = (i + random_double()) / (image_width - 1);
						auto v = (j + random_double()) / (image_height - 1);
						ray r = cam.get_ray(u, v);
						pixel_color += ray_color(r, world, max_depth);
					}
					write_color(out, pixel_color, samples_per_pixel);
				}
				else
				{
					auto u = double(i) / (image_width - 1);
					auto v = double(j) / (image_height - 1);
					ray r = cam.get_ray(u, v);
					vec3 pixel_color = ray_color(r, world, max_depth);
					write_color(out, pixel_color);
				}
			}
		}
		std::cerr << "\nDone.\n";
		return 0;
	}
	else
	{
		std::ofstream out("./image.ppm");
		if (!out)
		{
			std::cout << "Could not open file!" << std::endl;
			return -1;
		}

		out << "P3\n" << image_width << ' ' << image_height << "\n255\n";

		for (int j = image_height - 1; j >= 0; --j) {
			std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
			for (int i = 0; i < image_width; ++i) {
				if (use_antialiasing)
				{
					vec3 pixel_color(0, 0, 0);
					for (int s = 0; s < samples_per_pixel; ++s)
					{
						auto u = (i + random_double()) / (image_width - 1);
						auto v = (j + random_double()) / (image_height - 1);
						ray r = cam.get_ray(u, v);
						pixel_color += ray_color(r, world, max_depth);
					}
					write_color(out, pixel_color, samples_per_pixel);
				}
				else
				{
					auto u = double(i) / (image_width - 1);
					auto v = double(j) / (image_height - 1);
					ray r = cam.get_ray(u, v);
					vec3 pixel_color = ray_color(r, world, max_depth);
					write_color(out, pixel_color);
				}
			}
		}
		std::cerr << "\nDone.\n";
		return 0;
	}
	

	//std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

	//for (int j = image_height - 1; j >= 0; --j) {
	//	std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
	//	for (int i = 0; i < image_width; ++i) {

	//		auto u = double(i) / (image_width - 1);
	//		auto v = double(j) / (image_height - 1);
	//		ray r(origin, lower_left_corner + u * horizontal + v * vertical - origin);
	//		vec3 pixel_color = ray_color(r);

	//		//vec3 pixel_color(double(i) / (image_width - 1), double(j) / (image_height - 1), 0.25);
	//		write_color(std::cout, pixel_color);
	//	}
	//}
	//std::cerr << "\nDone.\n";

    return 0;
}