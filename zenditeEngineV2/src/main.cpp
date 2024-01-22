/*
#include "utils.h"
#include <GLFW/glfw3.h>

#include "vendor/stb_image/stb_image.h"
#include "assimp/Importer.hpp"

#include "Shader.h"
#include "Texture2D.h"
#include "geometrySetup.h"
#include "menu.h"
#include "Camera.h"

#include "Helper/Model.h"
*/

#include <iostream>

#include "RT/vec3.h"
#include "RT/color.h"

int main()
{
	int image_width = 256;
	int image_height = 256;


	//Render
	std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

	for (int j = 0; j < image_height; j++)
	{
		std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
		for (int i = 0; i < image_width; i++)
		{
			auto pixel_color = color(double(i) / (image_width - 1), double(j) / (image_height), 0);
			write_color(std::cout, pixel_color);
		}
	}

	std::clog << "\rDone. Check file for output            \n";

	return 0;
};