#include "material/Material.h"

Material::Material(const Shader& shader) : shader(shader)
{
    
}

const Shader& Material::GetShader(){
    return shader;
}