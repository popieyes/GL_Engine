#include "core/Engine.h"
#include "core/Editor.h"
#include "core/Event.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include "utils/Logger.h"

Editor::Editor(Engine* eng) : engine(eng) {
  if(engine == nullptr) {
    CORE_ERROR("Editor initialized with null engine pointer!");
  }
}
Editor::~Editor() {
  Shutdown();
}
void Editor::Init()
{
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); 

  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

  ImGui::StyleColorsDark();
  CORE_INFO("ImGui context created and style set.");
  GLFWwindow* window = engine->GetWindow();
  if (!window) {
    CORE_ERROR("Failed to get GLFW window from engine. ImGui initialization aborted.");
    return;
  }
  ImGui_ImplGlfw_InitForOpenGL(engine->GetWindow(), true);
  CORE_INFO("ImGui GLFW backend initialized.");
  ImGui_ImplOpenGL3_Init("#version 330");
}

void Editor::BeginFrame()
{
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  CreateEditorLayout();
}
void Editor::Render()
{
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        // Viewports need the current context to be backed up and restored
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
  
}


void Editor::Shutdown()
{
  if(ImGui::GetCurrentContext() != nullptr)
  {
    ImGui_ImplOpenGL3_Shutdown();
	  ImGui_ImplGlfw_Shutdown();
	  ImGui::DestroyContext();
  }
}

void Editor::CreateEditorLayout()
{
  static bool opt_fullscreen = true;
  static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

  ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
  if(opt_fullscreen)
  {
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

  }

  ImGui::Begin("DockSpace demo", nullptr, window_flags);
  if (opt_fullscreen)
  {
    ImGui::PopStyleVar(2);
  }

  ImGuiIO& io = ImGui::GetIO();
  if(io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
  {
    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
  }


  CreateSceneViewport();
  CreateGameViewport();
  CreateSceneHierarchy();
  CreateInspector();
  
  ImGui::End();
}

void Editor::CreateSceneViewport()
{
  // Force a minimum size so it doesn't collapse into a tiny square
  ImGui::SetNextWindowSizeConstraints(ImVec2(400 , 300), ImVec2(4000, 4000));
  ImGui::Begin("Scene Viewport");  

  bool isHovered = ImGui::IsWindowHovered();
  bool isRightClick = ImGui::IsMouseDown(ImGuiMouseButton_Right);

  static bool wasActive = false;
  bool isActive = isHovered && isRightClick;

  if(isActive != wasActive) {
    ViewportFocusEvent e(isActive);
    m_Dispatcher->Dispatch(e);
    wasActive = isActive;
  }
  ImVec2 viewport_panel_size = ImGui::GetContentRegionAvail();
  uint32_t textureID = engine->GetSceneTextureID();

  // Draw the texture. 
  // Note: OpenGL renders upside down relative to ImGui. 
  // We fix this by flipping the UV coordinates: (0, 1) and (1, 0).
  ImGui::Image(
  (ImTextureID)(intptr_t)textureID,
  viewport_panel_size,
  ImVec2(0, 1), 
  ImVec2(1, 0));

  ImGui::End();
}

void Editor::CreateGameViewport()
{
  ImGui::Begin("Game Viewport");
  ImGui::Text("Game view will be rendered here");
  ImGui::End();
}

void Editor::CreateSceneHierarchy()
{
  ImGui::Begin("Scene Hierarchy");
  ImGui::Text("No objects in the scene");
  ImGui::End();
} 

void Editor::CreateInspector()
{
  ImGui::Begin("Inspector");
  ImGui::Text("No object selected");
  ImGui::End();
}

