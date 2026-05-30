#pragma once
#include "core/Engine.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#if defined(__INTELLISENSE__) || !defined(USE_CPP20_MODULES)
#include <vulkan/vulkan_raii.hpp>
#else
import vulkan_hpp;
#endif

class VKEngine : public Engine {
  public:
    const int Setup() override;
    void Run() override;
    void BeginFrame() override;
    void EndFrame() override;

  private:
    vk::raii::Context context;
    vk::raii::Instance instance = nullptr;
    vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr;
    vk::raii::PhysicalDevice physicalDevice = nullptr;
    vk::raii::Device device = nullptr;
    vk::raii::SurfaceKHR surface = nullptr;
    vk::raii::Queue graphicsQueue = nullptr;

    void CreateInstance();
    void SetupDebugMessenger();
    void CreateSurface();
    void SelectPhysicalDevice();
    void CreateLogicalDevice();


    std::vector<const char*> GetRequiredInstanceExtensions();
    bool IsDeviceSuitable(vk::raii::PhysicalDevice const& device);
};