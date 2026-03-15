#include "utils/FrameBuffer.h"

struct GLFWwindow;
namespace ImGui {class Context;}

class Editor {
  public:
    Editor(FrameBuffer& fbo);
    ~Editor();

    Editor(const Editor&) = delete;
    Editor& operator=(const Editor&) = delete;

    void Init(GLFWwindow* window);
    void BeginFrame();
    void Render();
    void Shutdown();

  private:
    void CreateEditorLayout();
    void CreateSceneHierarchy();
    void CreateInspector();
    void CreateSceneViewport();
    void CreateGameViewport();
    FrameBuffer& scene_fbo;
};