#include "loaders/SceneLoader.h"
#include <iostream>

// Define static members
WindowSettings SceneLoader::windowSettings;
CameraSettings SceneLoader::cameraSettings;
std::vector<EntityConfig> SceneLoader::entities;

glm::vec3 SceneLoader::ParseVec3(const YAML::Node& node) {
    if(!node.IsSequence() || node.size() != 3) {
        std::cerr << "Warning: Invalid Vec3 format in YAML" << std::endl;
        return glm::vec3(0.0f);
    }
    return glm::vec3(node[0].as<float>(), node[1].as<float>(), node[2].as<float>());
}

glm::vec4 SceneLoader::ParseVec4(const YAML::Node& node) {
    if(!node.IsSequence() || node.size() != 4) {
        return glm::vec4(1.0f); // Default to white/opaque
    }
    return glm::vec4(node[0].as<float>(), node[1].as<float>(), node[2].as<float>(), node[3].as<float>());
}

bool SceneLoader::LoadScene(const std::string& filepath) {
    try {
        YAML::Node config = YAML::LoadFile(filepath);

        std::cout << "--- Loading Scene: " << filepath << " ---" << std::endl;

        // 1. Load Global Settings
        if (config["settings"]) {
            auto settings = config["settings"];
            if (settings["window"]) {
                windowSettings.width = settings["window"]["width"].as<int>();
                windowSettings.height = settings["window"]["height"].as<int>();
                windowSettings.title = settings["window"]["title"].as<std::string>();
                windowSettings.vsync = settings["window"]["vsync"].as<bool>();
            }
            if (settings["background_color"]) {
                windowSettings.clearColor = ParseVec4(settings["background_color"]);
            }
        }

        // 2. Load Camera
        if (config["camera"]) {
            auto cam = config["camera"];
            cameraSettings.position = ParseVec3(cam["position"]);
            cameraSettings.lookAt = ParseVec3(cam["look_at"]);
            cameraSettings.up = ParseVec3(cam["up"]);
            cameraSettings.mouse_sensitivity = cam["mouse_sensitivity"].as<float>();
            cameraSettings.yaw = cam["yaw"].as<float>();
            cameraSettings.pitch = cam["pitch"].as<float>();
            cameraSettings.movement_speed = cam["movement_speed"].as<float>();
            cameraSettings.fov = cam["fov"].as<float>();
        }

        // 3. Load Entities
        entities.clear();
        if (config["entities"]) {
            auto entityList = config["entities"];
            for (auto entityNode : entityList) {
                EntityConfig entity;
                
                // Basic info
                entity.name = entityNode["name"].as<std::string>();
                entity.type = entityNode["type"].as<std::string>();
                entity.material = entityNode["material"]["shader"].as<std::string>();
                // Transform
                if (entityNode["transform"]) {
                    entity.position = ParseVec3(entityNode["transform"]["position"]);
                    entity.rotation = ParseVec3(entityNode["transform"]["rotation"]);
                    entity.scale    = ParseVec3(entityNode["transform"]["scale"]);
                }

                // Specifics
                if (entity.type == "mesh" && entityNode["path"]) {
                    entity.meshPath = entityNode["path"].as<std::string>();
                }
                
                entities.push_back(entity);
                std::cout << "Loaded Entity: " << entity.name << std::endl;
            }
        }
        
        return true;
    }
    catch (YAML::ParserException& e) {
        std::cerr << "Failed to parse YAML: " << e.what() << std::endl;
        return false;
    }
    catch (std::exception& e) {
        std::cerr << "Error loading scene: " << e.what() << std::endl;
        return false;
    }
}