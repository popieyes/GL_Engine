#include "Camera.h"
Camera::Camera(glm::vec3 pos, glm::vec3 front, glm::vec3 up) : MovementSpeed(SPEED), Yaw(YAW), Pitch(PITCH), MouseSensitivity(SENSITIVITY)
{
	Position = pos;
	Front = front;
	WorldUp = up;
	Right = glm::normalize(glm::cross(Front, WorldUp));
	Up = glm::normalize(glm::cross(Right, Front));
	UpdateView();
}

void Camera::UpdateView()
{
	View = glm::lookAt(Position, Position + Front, Up);
}

void Camera::SetOrtographicProj(int viewportW, int viewportH)
{
	Projection = glm::ortho(0.0f, (float)viewportW, 0.0f, (float)viewportH, 0.1f, 100.0f);
}

void Camera::SetPerspectiveProj(int viewportW, int viewportH, float fov)
{
	Projection = glm::perspective(glm::radians(fov), (float)viewportW/(float)viewportH, 0.1f, 100.0f);
}

void Camera::ProcessKeyboard(CameraMovement movement, float deltaTime)
{
	float velocity = MovementSpeed * deltaTime;
	if (movement == FORWARD)
		Position += Front * velocity;
	if (movement == BACKWARD)
		Position -= Front * velocity;
	if (movement == LEFT)
		Position -= Right * velocity;
	if (movement == RIGHT)
		Position += Right * velocity;
	Position.y = 2.0f;
}

void Camera::ProcessMouse(float xOffset, float yOffset, GLboolean constrainPitch)
{
	xOffset *= MouseSensitivity;
	yOffset *= MouseSensitivity;

	Yaw = glm::mod(Yaw + xOffset, 360.0f);
	Pitch += yOffset;

	if (Pitch > 80.0f)
		Pitch = 80.0f;
	if (Pitch < -80.0f)
		Pitch = -80.0f;

	glm::vec3 direction;
	direction.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	direction.z = sin(glm::radians(Yaw));
	direction.y = sin(glm::radians(Pitch)) * cos(glm::radians(Pitch));
	Front = glm::normalize(direction);
	Right = glm::normalize(glm::cross(Front, WorldUp));
	Up = glm::normalize(glm::cross(Right, Front));
}