#pragma once
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <string>

GLFWimage CreateIcon(const std::string& path)
{
	int width, height, channels;
	stbi_set_flip_vertically_on_load(0);
	stbi_uc* data = nullptr;
	data = stbi_load(path.c_str(), &width, &height, &channels, 0);

	GLFWimage image{ width, height, (unsigned char*)data };

	stbi_image_free(data);

	return image;
}