#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <yaml-cpp/yaml.h>

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
};

class SceneLoader
{
public:
    static bool LoadScene(const std::string& filepath);
    
    static WindowSettings GetWindowSettings() { return windowSettings; }
    static CameraSettings GetCameraSettings() { return cameraSettings; }
    static std::vector<EntityConfig> GetEntities() { return entities; }
    
private:
    static glm::vec3 ParseVec3(const YAML::Node& node);
    static glm::vec4 ParseVec4(const YAML::Node& node);

    // Internal storage
    static WindowSettings windowSettings;
    static CameraSettings cameraSettings;
    static std::vector<EntityConfig> entities;
};
