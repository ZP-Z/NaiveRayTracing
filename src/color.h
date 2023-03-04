#pragma once

#include "vec3.h"
#include <iostream>
#include <fstream>

void write_color(std::ostream& out, vec3 pixel_color)
{
	// write the translated [0, 255] value of each color component
	out << static_cast<int>(255.999 * pixel_color.x()) << ' '
		<< static_cast<int>(255.999 * pixel_color.y()) << ' '
		<< static_cast<int>(255.999 * pixel_color.z()) << '\n';
}

void write_color(std::ofstream& out, vec3 pixel_color)
{
	// write the translated [0, 255] value of each color component
	out << static_cast<int>(255.999 * pixel_color.x()) << ' '
		<< static_cast<int>(255.999 * pixel_color.y()) << ' '
		<< static_cast<int>(255.999 * pixel_color.z()) << '\n';
}

void write_color(std::ostream& out, vec3 pixel_color, int samples_per_pixel)
{
	auto r = pixel_color.x();
	auto g = pixel_color.y();
	auto b = pixel_color.z();

	// divide the color
	auto scale = 1.0 / samples_per_pixel;
	r = std::sqrt(scale * r);
	g = std::sqrt(scale * g);
	b = std::sqrt(scale * b);
	
	// write color
	out << static_cast<int>(256 * clamp(r, 0.0, 0.1)) << ' '
		<< static_cast<int>(256 * clamp(g, 0.0, 0.1)) << ' '
		<< static_cast<int>(256 * clamp(b, 0.0, 0.1)) << '\n';
}

void write_color(std::ofstream& out, vec3 pixel_color, int samples_per_pixel)
{
	auto r = pixel_color.x();
	auto g = pixel_color.y();
	auto b = pixel_color.z();

	// divide the color
	auto scale = 1.0 / samples_per_pixel;
	r = std::sqrt(scale * r);
	g = std::sqrt(scale * g);
	b = std::sqrt(scale * b);

	// write color
	out << static_cast<int>(256 * clamp(r, 0.0, 0.999)) << ' '
		<< static_cast<int>(256 * clamp(g, 0.0, 0.999)) << ' '
		<< static_cast<int>(256 * clamp(b, 0.0, 0.999)) << '\n';
}