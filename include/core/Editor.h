#pragma once
#include "utils/FrameBuffer.h"
#include "Common.h"
struct GLFWwindow;
namespace ImGui {class Context;}
class Engine;

class Editor {
  public:
    Editor(Engine* engine);
    ~Editor();

    Editor(const Editor&) = delete;
    Editor& operator=(const Editor&) = delete;

    void Init();
    void BeginFrame();
    void Render();
    void Shutdown();

  private:
    Engine* engine;

    void CreateEditorLayout();
    void CreateSceneHierarchy();
    void CreateInspector();
    void CreateSceneViewport();
    void CreateGameViewport();
};