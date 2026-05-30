#pragma once
#include "utils/FrameBuffer.h"
#include "Common.h"

struct GLFWwindow;
class Engine;
namespace ImGui {class Context;}

class Editor {
  public:
    Editor(Engine* engine);
    virtual ~Editor();

    Editor(const Editor&) = delete;
    Editor& operator=(const Editor&) = delete;

    void Init();
    void BeginFrame();
    void Render();
    void Shutdown();

  protected:
    Engine* engine;
    virtual void OnInitBackend() = 0;
    virtual void OnBeginFrameBackend() = 0;
    virtual void OnRenderBackend() = 0;
    virtual void OnShutdownBackend() = 0;

  private:
    void CreateEditorLayout();
    void CreateSceneHierarchy();
    void CreateInspector();
    void CreateSceneViewport();
    void CreateGameViewport();
};