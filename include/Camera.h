#pragma once
#include "Common.h"

enum CameraMovement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

class Camera
{
public:
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 WorldUp;
	glm::vec3 Up;
	glm::vec3 Right;

	float Yaw;
	float Pitch;
	float MaxYaw;
	float MaxPitch;
	float MovementSpeed;
	float MouseSensitivity;
	
	Camera(glm::vec3 pos, glm::vec3 front, glm::vec3 up, float maxYaw, float maxPitch, float mouseSensitivity, float mouseMovementSpeed);
	
	void SetOrtographicProj(int viewportW, int viewportH);
	void SetPerspectiveProj(int viewportW, int viewportH, float fov);

	void UpdateView();
	void ProcessKeyboard(CameraMovement direction, float deltaTime);
	void ProcessMouse(float xOffset, float yOffset, GLboolean constrainPitch = true);

	const glm::mat4 GetView() const { return View; }
	const glm::mat4 GetProjection() const { return Projection; }
private:
	

	glm::mat4 View;
	glm::mat4 Projection;
};

