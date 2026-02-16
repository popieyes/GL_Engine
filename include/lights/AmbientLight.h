#pragma once
#include "Light.h"
class AmbientLight : public Light
{
public:
    void Setup(Shader& shader) override;
};
