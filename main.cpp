#pragma once
#include "Common.h"
#include "Scene.h"
#include "DirectionalLight.h"
#include "SpotLight.h"

#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 1200

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
void renderLoop(GLFWwindow* window)
{
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		context.deltaTime = currentFrame - context.lastFrame;
		context.lastFrame = currentFrame;

		processInput(window);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		mainScene.camera->UpdateView();
		mainScene.mainShader->SetMat4("view", mainScene.camera->GetView());

		float timeValue = glfwGetTime();
		float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
		
		glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
		
		/* Setup lights from the scene in the shader */
		for (Light* light : mainScene.lights)
		{
			light->Setup(*mainScene.mainShader);
		}

		/* Draw models. All info in the shader should be uploaded by this point. */
		for (unsigned int idx = 0; idx < mainScene.models.size(); idx++)
		{
			Model& gameObject = mainScene.models[idx];
			
			mainScene.mainShader->SetMat4("model", gameObject.ModelMat);
			gameObject.Draw(*mainScene.mainShader);
		}
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
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

GLFWwindow* createWindow(int& code)
{
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "GLGameEngine", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		code = -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	return window;
}

int main()
{
	int code = 0;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = createWindow(code);
	initGlad(code);
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	
	glEnable(GL_DEPTH_TEST);

	stbi_set_flip_vertically_on_load(true);

	mainScene = Scene();

	Model nanosuit(FileSystem::getPath("models/crysis/nanosuit.obj"));
	Model plane(FileSystem::getPath("models/plane/plane.obj"));
	Model backpack(FileSystem::getPath("models/backpack/backpack.obj"));

	backpack.ModelMat = glm::translate(backpack.ModelMat, glm::vec3(5.0f, 2.0f, 0.0f));
	/*context.models.push_back(nanosuit);
	context.models.push_back(plane);
	context.models.push_back(backpack);*/
	vector<Model> models = { nanosuit, plane, backpack };
	mainScene.AddModel(models);
	
	mainScene.mainShader = new Shader("shaders/phong.vert", "shaders/phong.frag");
	Shader& shader = *mainScene.mainShader;
	shader.Use();

	glm::vec3 cameraPos = glm::vec3(0.0f, 2.f, 10.0f);
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	mainScene.camera = new Camera(cameraPos, cameraPos + cameraFront, cameraUp);
	Camera& camera = *mainScene.camera;
	camera.SetPerspectiveProj(SCREEN_WIDTH, SCREEN_HEIGHT, 60.0f);
	shader.SetMat4("projection", camera.GetProjection());
	shader.SetVec3("View_Pos", camera.Position); // Needs to be updated in Render Loop

	
	/* LIGHTING */
	DirectionalLight* dirLight = new DirectionalLight(glm::vec3(0.0f, 5.0f, 1.0f));
	dirLight->Color = glm::vec3(1.0f, 1.f, 1.f);
	dirLight->Intensity = 1.0f;

	SpotLight* flashlight = new SpotLight();

	mainScene.AddLight(dirLight);
	mainScene.AddLight(flashlight);

	renderLoop(window);

	return code;
}