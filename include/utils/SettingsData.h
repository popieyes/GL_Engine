#pragma once
#include <string>
#include <glm/glm.hpp>

struct WindowSettings
{
    int width, height;
    std::string title;
    bool fullscreen;
    bool vsync;
    glm::vec4 clearColor;
};

struct CameraSettings
{
    glm::vec3 position;
    glm::vec3 lookAt;
    glm::vec3 up;
    float fov;
    float mouse_sensitivity;
    float yaw;
    float pitch;
    float movement_speed;
};

struct EntityConfig {
    std::string name;
    std::string type; // "mesh" or "light"
    std::string meshPath;
    std::string material;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    glm::vec4 color;
};
