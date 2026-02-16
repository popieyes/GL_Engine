#include "lights/DirectionalLight.h"


void DirectionalLight::Setup(Shader& shader) {
	shader.SetVec3("dirLight.Color", Color * Intensity);
	shader.SetVec3("dirLight.Dir", Direction );
}