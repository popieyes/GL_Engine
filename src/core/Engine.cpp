#include "loaders/SceneLoader.h"
#include "core/Engine.h"
#include "core/EventDispatcher.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stb_image.h>
#include "Scene.h"
#include "Camera.h"
#include "Shader.h"
#include "Model.h"
#include "lights/DirectionalLight.h"
#include "lights/SpotLight.h"
#include "lights/AmbientLight.h"
#include "material/Material.h"
#include "textures/CubeMap.h"
#include "utils/Logger.h"

Engine::Engine()
{
  // Load scene configuration
	if (!SceneLoader::LoadScene("default_scene.yaml")) {
    CORE_ERROR("Failed to load scene configuration!");
		return;
	}
  CORE_INFO("Scene configuration loaded successfully.");
  
	window_settings = SceneLoader::GetWindowSettings();
  glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window = glfwCreateWindow(window_settings.width, window_settings.height, window_settings.title.c_str(), NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
	}
	glfwMakeContextCurrent(window);
  glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
	glfwSetCursorPosCallback(window, MouseCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
	}
	glViewport(0, 0, window_settings.width, window_settings.height);
	
	glEnable(GL_DEPTH_TEST);

  stbi_set_flip_vertically_on_load(true);

  activeScene = new Scene();
  scene_fbo = new FrameBuffer(window_settings.width, window_settings.height);
  // scene_fbo.Resize(window_settings.width, window_settings.height); 
}

Engine::~Engine() {}

const int Engine::Setup() {

	dispatcher.Subscribe(EventType::ViewportActive, [this](Event& e)) {
		auto& focusEvent = static_cast<ViewportFocusEvent>(e);
		this-> isCameraLocked = focusEvent.IsActive;

		if(focusEvent.IsActive) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}

  /* ====== SCENE DEFINITION ======= */
	
	activeScene->mainShader = new Shader("assets/shaders/phong.vert", "assets/shaders/phong.frag");
	Shader& phong_shader = *activeScene->mainShader;
	Shader* toon_shader = new Shader("assets/shaders/toon.vert", "assets/shaders/toon.frag");
	skyboxShader = new Shader("assets/shaders/skybox.vert", "assets/shaders/skybox.frag");
	/* ====== CAMERA SETTINGS ====== */
	CameraSettings camera_settings = SceneLoader::GetCameraSettings();
	
	activeScene->camera = new Camera(
		camera_settings.position, 
		camera_settings.position + camera_settings.lookAt, 
		camera_settings.up,
		camera_settings.yaw,
		camera_settings.pitch,
		camera_settings.mouse_sensitivity,
		camera_settings.movement_speed);
		
		Camera& camera = *activeScene->camera;
		camera.SetPerspectiveProj(window_settings.width, window_settings.height, camera_settings.fov);
	
	phong_shader.Use();
	phong_shader.SetMat4("projection", camera.GetProjection());
	phong_shader.SetVec3("View_Pos", camera.Position); // Needs to be updated in Render Loop

	toon_shader->Use();
	toon_shader->SetMat4("projection", camera.GetProjection());
	toon_shader->SetVec3("View_Pos", camera.Position);

	Material* toon_mat = new Material(*toon_shader);
	Material* phong_mat = new Material(phong_shader);
	
	/* ===== ENTITIES LOAD ===== */
	const vector<EntityConfig> entities_config = SceneLoader::GetEntities();
	for (const EntityConfig& config : entities_config)
	{
		Model new_model(FileSystem::getPath(config.meshPath), config.material == "standard" ? *phong_mat : *toon_mat);
		new_model.material.Color = config.color; 
		new_model.ModelMat = scale(new_model.ModelMat, config.scale);
		new_model.ModelMat = translate(new_model.ModelMat, config.position);
		/*new_model->ModelMat = glm::rotate(glm::mat4(1.0f), entity.rotation);*/
		activeScene->AddModel(new_model);
	}

	/* ====== LIGHTING ====== */
	DirectionalLight* dirLight = new DirectionalLight(glm::vec3(0.0f, -1.0f, -0.5f));
	dirLight->Color = glm::vec3(1.0f, 1.f, 1.f);
	dirLight->Intensity = 1.f;

	SpotLight* flashlight = new SpotLight();
	AmbientLight* ambient_light = new AmbientLight();
	ambient_light->Intensity = 0.4f;
	ambient_light->Color = glm::vec3(1.0f, 1.0f, 1.0f);
	activeScene->AddLight(dirLight);
	activeScene->AddLight(flashlight);
	activeScene->AddLight(ambient_light);
	
	/* ==== SKYBOX ==== */
	vector<std::string> skybox_faces = {
		"assets/images/skybox/right.jpg",
		"assets/images/skybox/left.jpg",
		"assets/images/skybox/top.jpg",
		"assets/images/skybox/bottom.jpg",
		"assets/images/skybox/front.jpg",
		"assets/images/skybox/back.jpg"
	};
  skybox = new CubeMap(512, 512, 3, skybox_faces);

	/* ==== END ==== */
  return 0;
}

void Engine::Run() {

  float currentFrame = static_cast<float>(glfwGetTime());
  deltaTime = currentFrame - lastFrame;
  lastFrame = currentFrame;
  // Main game loop logic here

  activeScene->camera->UpdateView();
  activeScene->mainShader->Use();
  activeScene->mainShader->SetMat4("view", activeScene->camera->GetView());
  activeScene->mainShader->SetVec3("View_Pos", activeScene->camera->Position);
		/* Setup lights from the scene in the shader */
		

		/* Draw models. All info in the shader should be uploaded by this point. */
		for (unsigned int idx = 0; idx < activeScene->models.size(); idx++)
		{
			Model& gameObject = activeScene->models[idx];
			gameObject.material.GetShader().Use();
			gameObject.material.GetShader().SetMat4("model", gameObject.ModelMat);
			gameObject.material.GetShader().SetMat4("view", activeScene->camera->GetView());
			gameObject.material.GetShader().SetVec3("View_Pos", activeScene->camera->Position);
			gameObject.material.GetShader().SetVec4("material_color", gameObject.material.Color);
			
			for (Light* light : activeScene->lights)
			{
				light->Setup(gameObject.material.GetShader());
			}
			gameObject.Draw();
		}

    /* ==== RENDER SKYBOX ==== */
    glDepthFunc(GL_LEQUAL);
    skyboxShader->Use();
    skyboxShader->SetMat4("view", glm::mat4(glm::mat3(activeScene->camera->GetView())));
    skyboxShader->SetMat4("projection", activeScene->camera->GetProjection());
    glBindVertexArray(skybox->VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->ID);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
  
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    /* ==== END ==== */
}

void Engine::Shutdown() {
  glfwTerminate();
}

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


void Engine::EndFrame()
{
  glfwSwapBuffers(window);
  glfwPollEvents();
}

void Engine::BeginFrame(){
  glBindFramebuffer(GL_FRAMEBUFFER, scene_fbo->ID);

  glClearColor(window_settings.clearColor.r, window_settings.clearColor.g, window_settings.clearColor.b, window_settings.clearColor.a);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Engine::ProcessInput()
{
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
  activeScene->camera->ProcessKeyboard(FORWARD, deltaTime);
if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
  activeScene->camera->ProcessKeyboard(BACKWARD, deltaTime);
if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
  activeScene->camera->ProcessKeyboard(LEFT, deltaTime);
if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
  activeScene->camera->ProcessKeyboard(RIGHT, deltaTime);
if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
  glfwSetWindowShouldClose(window, true);
}