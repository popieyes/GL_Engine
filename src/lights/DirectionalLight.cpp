#include "lights/DirectionalLight.h"


void DirectionalLight::Setup(const Shader& shader) const
{
	shader.SetVec3("dirLight.Color", Color * Intensity);
	shader.SetVec3("dirLight.Dir", Direction );
}