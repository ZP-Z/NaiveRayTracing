#include "defines.h"

#include "color.h"
#include "hittble_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"

#include <tbb/tbb.h>
#include <tbb/parallel_for.h>
#include <tbb/parallel_reduce.h>
#include <tbb/parallel_pipeline.h>
#include <iostream>
#include <fstream>
#include <chrono>

hittble_list random_scene()
{
	hittble_list world;
	auto mat_ground = std::make_shared<lambertian>(vec3(0.5, 0.5, 0.5));
	world.add(std::make_shared<sphere>(vec3(0, -1000, 0), 1000, mat_ground));
	
	for (int a = -11; a < 11; ++a)
	{
		for (int b = -11; b < 11; ++b)
		{
			auto choose_mat = random_double();
			vec3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

			if ((center - vec3(4, 0.2, 0)).length() > 0.9)
			{
				std::shared_ptr<material> mat_sphere;

				if (choose_mat < 0.8)
				{
					// diffuse
					auto albedo = vec3::random() * vec3::random();
					mat_sphere = std::make_shared<lambertian>(albedo);
					world.add(std::make_shared<sphere>(center, 0.2, mat_sphere));
				}
				else if (choose_mat < 0.95)
				{
					// metal
					auto albedo = vec3::random(0.5, 1);
					auto fuzz = random_double(0, 0.5);
					mat_sphere = std::make_shared<metal>(albedo, fuzz);
					world.add(std::make_shared<sphere>(center, 0.2, mat_sphere));
				}
				else
				{
					// glass
					mat_sphere = std::make_shared<dielectric>(1.5);
					world.add(std::make_shared<sphere>(center, 0.2, mat_sphere));
				}
			}
		}
	}

	auto mat_1 = std::make_shared<dielectric>(1.5);
	world.add(std::make_shared<sphere>(vec3(0, 1, 0), 1.0, mat_1));

	auto mat_2 = std::make_shared<lambertian>(vec3(0.4, 0.2, 0.1));
	world.add(std::make_shared<sphere>(vec3(-4, 1, 0), 1.0, mat_2));

	auto mat_3 = std::make_shared<metal>(vec3(0.7, 0.6, 0.5), 0.0);
	world.add(std::make_shared<sphere>(vec3(4, 1, 0), 1.0, mat_3));

	return world;
}

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
		/*vec3 target = rec.p + random_in_hemisphere(rec.normal);
		return 0.5 * ray_color(ray(rec.p, target - rec.p), world, depth - 1);*/

		// material
		ray scattered_ray;
		vec3 attenuation;
		if (rec.mat->scatter(r, rec, attenuation, scattered_ray))
			return attenuation * ray_color(scattered_ray, world, depth - 1);
		return vec3(0, 0, 0); // hit the back of object
	}

	// blue background
	vec3 unit_direction = normalize(r.direction());
	auto t = 0.5 * (unit_direction.y() + 1.0);
	return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
}

int main(int argc, char* argv)
{
    // Image
	const auto aspect_ratio = 3.0 / 2.0;
    const int image_width = 1200;
	const int image_height = static_cast<int>(image_width / aspect_ratio);
	const int samples_per_pixel = 500;
	const int max_depth = 50;

	// World
	auto world = random_scene();

	// Camera
	vec3 lookfrom(13, 2, 3);
	vec3 lookat(0, 0, 0);
	vec3 vup(0, 1, 0);
	auto dist_to_focus = 10.0;
	auto aperture = 0.1;

	camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);

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

		int parallel_method = 1;

		// parallel do ray tracing
		if (parallel_method == 1)
		{
			/////////////////////////////////////////////
			std::vector<vec3> pixel_colors(image_width * image_height, vec3(0, 0, 0));
			std::atomic<size_t> atomic_progress_counter;
			auto time_now = std::chrono::steady_clock::now();

			std::cout << "image height: " << image_height << ", image_width: " << image_width << std::endl;

			tbb::parallel_for(size_t(0), size_t(image_height), [&](size_t inv_j) {
				int j = image_height - 1 - inv_j;
				atomic_progress_counter++;
				std::cerr << "\rScanlines remaining: " << image_height - 1 - atomic_progress_counter << "      " << std::flush;

				tbb::parallel_for(size_t(0), size_t(image_width), [&](size_t i) {
					vec3 pixel_color(0, 0, 0);
					int do_sample_step = 0;

					tbb::parallel_pipeline(14,
						tbb::make_filter<void, void>(
							tbb::detail::d1::filter_mode::serial_in_order,
							[&](tbb::flow_control& fc) {
								if (do_sample_step < samples_per_pixel)
								{
									do_sample_step++;
								}
								else {
									fc.stop();
								}
							}) &
						tbb::make_filter<void, vec3>(
							tbb::detail::d1::filter_mode::parallel,
							[&](tbb::flow_control& fc)-> vec3 {
								auto u = (i + random_double()) / (image_width - 1);
							auto v = (j + random_double()) / (image_height - 1);
							ray r = cam.get_ray(u, v);
							return ray_color(r, world, max_depth);
							}
							) &
								tbb::make_filter<vec3, void>(
									tbb::detail::d1::filter_mode::serial_out_of_order,
									[&](vec3 x) {
										pixel_color += x;
									})
								);
					pixel_colors[inv_j * image_width + i] = pixel_color;
					});
				});
			double time_cost = std::chrono::duration<double>(std::chrono::steady_clock::now() - time_now).count();
			std::cout << std::endl << "time cost: " << int(time_cost) / 60 << "m, " << int(time_cost) % 60 << "s" << std::endl;
			for (auto& pixel_color : pixel_colors)
			{
				write_color(out, pixel_color, samples_per_pixel);
			}
		}
		else if (parallel_method == 2)
		{
			auto time_now = std::chrono::steady_clock::now();
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
			double time_cost = std::chrono::duration<double>(std::chrono::steady_clock::now() - time_now).count();
			std::cout << std::endl << "time cost: " << int(time_cost) / 60 << "m, " << int(time_cost) % 60 << "s" << std::endl;
		}


		out.close();
		std::cerr << "\nDone.\n";
		return 0;
	}

    return 0;
}