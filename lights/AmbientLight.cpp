#include "AmbientLight.h"

void AmbientLight::Setup(Shader& shader)
{
    shader.SetFloat("ambient_light.Intensity", Intensity);
    shader.SetVec3("ambient_light.Color", Color);
}
