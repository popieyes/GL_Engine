#include "Common.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "Model.h"

#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 1200

int vertexColorLocation;

struct AppContext {
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	float mouseLastX;
	float mouseLastY;
	bool mouseInWindow = false;

	Shader* shader;
	Camera* camera;
	vector<Model> models;
};

AppContext context;

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		context.camera->ProcessKeyboard(FORWARD, context.deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		context.camera->ProcessKeyboard(BACKWARD, context.deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		context.camera->ProcessKeyboard(LEFT, context.deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		context.camera->ProcessKeyboard(RIGHT, context.deltaTime);
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}
void renderLoop(GLFWwindow* window, Shader shader)
{
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		context.deltaTime = currentFrame - context.lastFrame;
		context.lastFrame = currentFrame;

		processInput(window);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		context.camera->UpdateView();
		context.shader->SetMat4("view", context.camera->GetView());

		float timeValue = glfwGetTime();
		float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
		
		glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
		
		for (unsigned int idx = 0; idx < context.models.size(); idx++)
		{
			Model& gameObject = context.models[idx];
			shader.SetMat4("model", gameObject.ModelMat);
			gameObject.Draw(shader);
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

	context.camera->ProcessMouse(xOffset, yOffset);
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

	Model nanosuit(FileSystem::getPath("models/crysis/nanosuit.obj"));
	Model plane(FileSystem::getPath("models/plane/plane.obj"));
	Model backpack(FileSystem::getPath("models/backpack/backpack.obj"));

	backpack.ModelMat = glm::translate(backpack.ModelMat, glm::vec3(5.0f, 2.0f, 0.0f));
	context.models.push_back(nanosuit);
	context.models.push_back(plane);
	context.models.push_back(backpack);
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	
	context.shader = new Shader("shaders/phong.vert", "shaders/phong.frag");
	Shader& shader = *context.shader;
	shader.Use();

	glm::vec3 cameraPos = glm::vec3(0.0f, 2.f, 10.0f);
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	context.camera = new Camera(cameraPos, cameraPos + cameraFront, cameraUp);
	Camera& camera = *context.camera;
	camera.SetPerspectiveProj(SCREEN_WIDTH, SCREEN_HEIGHT, 60.0f);

	
	shader.SetMat4("projection", camera.GetProjection());
	shader.SetVec3("Light_Color", glm::vec3(1.0f, 1.0f, 1.0f));
	shader.SetVec3("Light_Pos", glm::vec3(0.0f, 5.0f, 5.0f));
	shader.SetVec3("View_Pos", camera.Position); // Needs to be updated in Render Loop

	vertexColorLocation = glGetUniformLocation(shader.ID, "ourColor");

	renderLoop(window, shader);

	return code;
}