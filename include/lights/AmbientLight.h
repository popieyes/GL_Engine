#pragma once
#include "Light.h"
class AmbientLight : public Light
{
public:
    void Setup(const Shader& shader) const override;
};
