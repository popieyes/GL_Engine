#include "Camera.h"
Camera::Camera(glm::vec3 pos, glm::vec3 front, glm::vec3 up, float maxYaw, float maxPitch, float mouseSensitivity, float mouseMovementSpeed)
{
	Position = pos;
	Front = front;
	WorldUp = up;
	Right = glm::normalize(glm::cross(Front, WorldUp));
	Up = glm::normalize(glm::cross(Right, Front));
	MovementSpeed = mouseMovementSpeed;
	MouseSensitivity = mouseSensitivity;
	MaxPitch = maxPitch;
	MaxYaw = maxYaw;
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
	
	Pitch = std::max(std::min(Pitch, MaxPitch), -MaxPitch);
	
	glm::vec3 direction;
	
	float rYaw = glm::radians(Yaw);
	float rPitch = glm::radians(Pitch);

	// Compute Y (height)
	// Depends on how high or low you are aiming
	direction.y = sin(rPitch); 

	// Compute the shadow of the proyection in the floor
	// If you look way too high it reaches 0
	float shadow = cos(rPitch);

	// Compute X and Z using the shadow
	direction.x = cos(rYaw) * shadow; // cos(yaw) * cos(pitch)
	direction.z = sin(rYaw) * shadow; // sin(yaw) * cos(pitch)
	
	
	Front = glm::normalize(direction);
	Right = glm::normalize(glm::cross(Front, WorldUp));
	Up = glm::normalize(glm::cross(Right, Front));
}