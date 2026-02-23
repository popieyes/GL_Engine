#pragma once
#include "../Common.h"
#include "../Shader.h"

class Light
{
public:
	Light() {}
	vec3 Color = vec3(1.0f);
	float Intensity = 1.0f;
	bool castShadows = true;

	virtual void Setup(const Shader& shader) const {}
};

