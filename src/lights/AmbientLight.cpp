#include "lights/AmbientLight.h"

void AmbientLight::Setup(const Shader& shader) const
{
    shader.SetFloat("ambient_light.Intensity", Intensity);
    shader.SetVec3("ambient_light.Color", Color);
}
