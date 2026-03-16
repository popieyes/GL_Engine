#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <yaml-cpp/yaml.h>
#include "utils/SettingsData.h"

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
