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
	
	Pitch = std::max(std::min(Pitch, 80.f), -80.0f);
	
	glm::vec3 direction;

	// Convertir a radianes (glm usa radianes, no grados)
	float rYaw = glm::radians(Yaw);
	float rPitch = glm::radians(Pitch);

	// Calcular Y (Altura)
	// Solo depende de cuanto miras arriba/abajo
	direction.y = sin(rPitch); 

	// Calcular la longitud de la proyeccion en el suelo (la "sombra")
	// Si miras muy arriba, esto se acerca a 0.
	float sombra = cos(rPitch);

	// Calcular X y Z usando esa sombra
	direction.x = cos(rYaw) * sombra; // cos(yaw) * cos(pitch)
	direction.z = sin(rYaw) * sombra; // sin(yaw) * cos(pitch)
	
	
	Front = glm::normalize(direction);
	Right = glm::normalize(glm::cross(Front, WorldUp));
	Up = glm::normalize(glm::cross(Right, Front));
}