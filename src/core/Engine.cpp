#include "loaders/SceneLoader.h"
#include "core/Engine.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include "utils/Logger.h"
#include "Scene.h"
Engine::Engine()
{
  if (!SceneLoader::LoadScene(ROOT_DIR "default_scene.yaml")) {
    CORE_ERROR("Failed to load scene configuration!");
    return;
  }
  CORE_INFO("Scene configuration loaded successfully.");
  window_settings = SceneLoader::GetWindowSettings();
}

Engine::~Engine() {}


const bool Engine::IsRunning() const {
  return !glfwWindowShouldClose(window);
}

void Engine::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
  auto* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
  if (engine) {
    engine->OnResize(window, width, height);
  }

}

void Engine::MouseCallback(GLFWwindow* window, double xPosIn, double yPosIn)
{
  auto* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
  if (engine) {
    engine->OnMouse(window, xPosIn, yPosIn);
  }
}

void Engine::OnResize(GLFWwindow* window, int width, int height)
{
  glViewport(0, 0, width, height);
}

void Engine::OnMouse(GLFWwindow* window, double xPosIn, double yPosIn) 
{
  float xPos = static_cast<float>(xPosIn);
	float yPos = static_cast<float>(yPosIn);
	if (!mouseInWindow)
	{
		mouseLastX = xPos;
		mouseLastY = yPos;
		mouseInWindow = true;
	}

	float xOffset = xPos - mouseLastX;
	float yOffset = mouseLastY - yPos;

	mouseLastX = xPos;
	mouseLastY = yPos;

	activeScene->camera->ProcessMouse(xOffset, yOffset);
}
