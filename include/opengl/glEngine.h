#pragma once
#include "core/Engine.h"
#include "GLFW/glfw3.h"

class GLEngine : public Engine {
  public:
    GLEngine();

    const int Setup() override;
    void BeginFrame() override;
    void EndFrame() override;
    void Run() override;
    void Shutdown() override;
    void ProcessInput() override;
};