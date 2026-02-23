#pragma once
#include <glad/glad.h>
#include "stb_image.h"
#include <iostream>
#include <vector>

class CubeMap
{
public:
  CubeMap(int width, int height, int nrChannels, std::vector<std::string> textures_faces);
  unsigned int ID;
  unsigned int VAO, VBO;
};