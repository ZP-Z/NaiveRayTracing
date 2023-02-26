#include "defines.h"

#include "color.h"
#include "hittble_list.h"
#include "sphere.h"

#include <iostream>
#include <fstream>

// ray tracing for the objects in the world
vec3 ray_color(const ray& r, const hittable& world)
{
	// when hit the object
	hit_record rec;
	if (world.hit(r, 0, BIG_NUMBER, rec))
	{
		return 0.5 * (rec.normal + vec3(1, 1, 1));
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

	// World
	hittble_list world;
	world.add(std::make_shared<sphere>(vec3(0, 0, -1), 0.5));
	world.add(std::make_shared<sphere>(vec3(0, -100.5, -1), 100));

	// Camera
	auto viewport_height = 2.0;
	auto viewport_width = aspect_ratio * viewport_height;
	auto focal_length = 1.0;

	auto origin = vec3(0, 0, 0);
	auto horizontal = vec3(viewport_width, 0, 0);
	auto vertical = vec3(0, viewport_height, 0);
	auto lower_left_corner = origin - horizontal / 2 - vertical / 2 - vec3(0, 0, focal_length);

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

				auto u = double(i) / (image_width - 1);
				auto v = double(j) / (image_height - 1);
				ray r(origin, lower_left_corner + u * horizontal + v * vertical - origin);
				vec3 pixel_color = ray_color(r, world);

				//vec3 pixel_color(double(i) / (image_width - 1), double(j) / (image_height - 1), 0.25);
				write_color(out, pixel_color);
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

				auto u = double(i) / (image_width - 1);
				auto v = double(j) / (image_height - 1);
				ray r(origin, lower_left_corner + u * horizontal + v * vertical - origin);
				vec3 pixel_color = ray_color(r, world);

				//vec3 pixel_color(double(i) / (image_width - 1), double(j) / (image_height - 1), 0.25);
				write_color(out, pixel_color);
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