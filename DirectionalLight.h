#pragma once
#include "Light.h"
class DirectionalLight : public Light
{
public:
	DirectionalLight(vec3 dir) { Direction = dir; }

	void Setup(Shader& shader) override;
private:
	vec3 Direction;
};

