#pragma once
#include "stb_image.h"
#include <iostream>
#include <glad/glad.h>
class TextureLoader
{
public:
	unsigned int ID;

	TextureLoader(const char* texturePath, const int sourceChannels);

	void use();
private:
	int width, height, nrChannels;
};

