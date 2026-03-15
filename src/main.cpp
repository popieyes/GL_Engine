#include "Common.h"
#include "Scene.h"
#include "lights/DirectionalLight.h"
#include "lights/SpotLight.h"
#include "lights/AmbientLight.h"
#include <yaml-cpp/yaml.h>
#include "loaders/SceneLoader.h"
#include "material/Material.h"
#include "textures/CubeMap.h"
#include "ui/Editor.h"
#include "utils/FrameBuffer.h"

int vertexColorLocation;

struct AppContext {
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	float mouseLastX;
	float mouseLastY;
	bool mouseInWindow = false;
};

AppContext context;
Scene mainScene;

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		mainScene.camera->ProcessKeyboard(FORWARD, context.deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		mainScene.camera->ProcessKeyboard(BACKWARD, context.deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		mainScene.camera->ProcessKeyboard(LEFT, context.deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		mainScene.camera->ProcessKeyboard(RIGHT, context.deltaTime);
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void initGlad(int& code)
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		code = -1;
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xPosIn, double yPosIn)
{
	float xPos = static_cast<float>(xPosIn);
	float yPos = static_cast<float>(yPosIn);
	if (!context.mouseInWindow)
	{
		context.mouseLastX = xPos;
		context.mouseLastY = yPos;
		context.mouseInWindow = true;
	}

	float xOffset = xPos - context.mouseLastX;
	float yOffset = context.mouseLastY - yPos;

	context.mouseLastX = xPos;
	context.mouseLastY = yPos;

	mainScene.camera->ProcessMouse(xOffset, yOffset);
}

GLFWwindow* createWindow(int& code, const WindowSettings& settings)
{
	GLFWwindow* window = glfwCreateWindow(settings.width, settings.height, settings.title.c_str(), NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		code = -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	return window;
}


int main()
{
	// Load scene configuration
	if (!SceneLoader::LoadScene("default_scene.yaml")) {
		return -1;
	}
	WindowSettings window_settings = SceneLoader::GetWindowSettings();
	
	int code = 0;
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = createWindow(code, window_settings);
	initGlad(code);
	glViewport(0, 0, window_settings.width, window_settings.height);
	
	glEnable(GL_DEPTH_TEST);

	stbi_set_flip_vertically_on_load(true);
	
	FrameBuffer scene_fbo(window_settings.width, window_settings.height);

	Editor editor(scene_fbo);
	editor.Init(window);

	/* ====== SCENE DEFINITION ======= */
	
	mainScene = Scene();
	
	mainScene.mainShader = new Shader("assets/shaders/phong.vert", "assets/shaders/phong.frag");
	Shader& phong_shader = *mainScene.mainShader;
	Shader* toon_shader = new Shader("assets/shaders/toon.vert", "assets/shaders/toon.frag");
	Shader& skyboxShader = *new Shader("assets/shaders/skybox.vert", "assets/shaders/skybox.frag");
	/* ====== CAMERA SETTINGS ====== */
	CameraSettings camera_settings = SceneLoader::GetCameraSettings();
	
	mainScene.camera = new Camera(
		camera_settings.position, 
		camera_settings.position + camera_settings.lookAt, 
		camera_settings.up,
		camera_settings.yaw,
		camera_settings.pitch,
		camera_settings.mouse_sensitivity,
		camera_settings.movement_speed);
		
		Camera& camera = *mainScene.camera;
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
		mainScene.AddModel(new_model);
	}

	/* ====== LIGHTING ====== */
	DirectionalLight* dirLight = new DirectionalLight(glm::vec3(0.0f, -1.0f, -0.5f));
	dirLight->Color = glm::vec3(1.0f, 1.f, 1.f);
	dirLight->Intensity = 1.f;

	SpotLight* flashlight = new SpotLight();
	AmbientLight* ambient_light = new AmbientLight();
	ambient_light->Intensity = 0.4f;
	ambient_light->Color = glm::vec3(1.0f, 1.0f, 1.0f);
	mainScene.AddLight(dirLight);
	mainScene.AddLight(flashlight);
	mainScene.AddLight(ambient_light);
	
	/* ==== SKYBOX ==== */
	vector<std::string> skybox_faces = {
		"assets/images/skybox/right.jpg",
		"assets/images/skybox/left.jpg",
		"assets/images/skybox/top.jpg",
		"assets/images/skybox/bottom.jpg",
		"assets/images/skybox/front.jpg",
		"assets/images/skybox/back.jpg"
	};
	CubeMap skybox(512, 512, 3, skybox_faces);
	/* ==== END ==== */
	
	/* ==== RENDER LOOP ==== */
	while (!glfwWindowShouldClose(window))
	{
		editor.BeginFrame();

		float currentFrame = static_cast<float>(glfwGetTime());
		context.deltaTime = currentFrame - context.lastFrame;
		context.lastFrame = currentFrame;

		processInput(window);

		glBindFramebuffer(GL_FRAMEBUFFER, scene_fbo.ID);

		glClearColor(window_settings.clearColor.r, window_settings.clearColor.g, window_settings.clearColor.b, window_settings.clearColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		

		mainScene.camera->UpdateView();
		mainScene.mainShader->Use();
		mainScene.mainShader->SetMat4("view", mainScene.camera->GetView());
		mainScene.mainShader->SetVec3("View_Pos", mainScene.camera->Position);
		/* Setup lights from the scene in the shader */
		

		/* Draw models. All info in the shader should be uploaded by this point. */
		for (unsigned int idx = 0; idx < mainScene.models.size(); idx++)
		{
			Model& gameObject = mainScene.models[idx];
			gameObject.material.GetShader().Use();
			gameObject.material.GetShader().SetMat4("model", gameObject.ModelMat);
			gameObject.material.GetShader().SetMat4("view", mainScene.camera->GetView());
			gameObject.material.GetShader().SetVec3("View_Pos", mainScene.camera->Position);
			gameObject.material.GetShader().SetVec4("material_color", gameObject.material.Color);
			/* mainScene.mainShader->SetMat4("model", gameObject.ModelMat); */
			
			for (Light* light : mainScene.lights)
			{
				light->Setup(gameObject.material.GetShader());
			}
			gameObject.Draw();
		}
		/* ==== RENDER SKYBOX ==== */
		glDepthFunc(GL_LEQUAL);
		skyboxShader.Use();
		skyboxShader.SetMat4("view", glm::mat4(glm::mat3(mainScene.camera->GetView())));
		skyboxShader.SetMat4("projection", mainScene.camera->GetProjection());
		glBindVertexArray(skybox.VAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.ID);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		/* ==== END ==== */
		editor.Render();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	editor.Shutdown();
	/* ==== END ==== */
	/* renderLoop(window, window_settings); */

	return code;
}