#pragma once
#include "Shader.h"
#include <variant>
#include <map>
#include <string>

using Material_Value = std::variant<float, int, glm::vec3, glm::vec4>;

class Material 
{

public:
    Material(const Shader& shader);
    void Apply();
    const Shader& GetShader();
private:
    const Shader& shader;
    std::map<std::string, Material_Value> parameters;
};