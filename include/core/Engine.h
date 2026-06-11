#pragma once
#include "utils/FrameBuffer.h"
#include "utils/SettingsData.h"
#include "Shader.h"
#include "textures/CubeMap.h"

struct GLFWwindow;
class Scene;
class Editor;


class Engine {
  public:
    Engine();
    ~Engine();

    virtual const int Setup() = 0;
    virtual void BeginFrame() = 0;
    virtual void EndFrame() = 0;
    virtual void Run() = 0;
    virtual void Shutdown() = 0;
    virtual void ProcessInput() = 0;
    
    const bool IsRunning() const;
    GLFWwindow* GetWindow() const { return window; }

    const unsigned int GetSceneTextureID() const { return scene_fbo->GetColorTextureID(); }

  protected:
    GLFWwindow* window;
    WindowSettings window_settings;
    FrameBuffer* scene_fbo; 
    Editor* editor;
    
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    
    float mouseLastX = 0.0f;
    float mouseLastY = 0.0f;
    bool mouseInWindow = false;

    Scene* activeScene;
    Shader* skyboxShader;
    CubeMap* skybox;
    static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void MouseCallback(GLFWwindow* window, double xPosIn, double yPosIn);
    
    void OnResize(GLFWwindow* window, int width, int height);
    void OnMouse(GLFWwindow* window, double xPosIn, double yPosIn);

    void Render();
};