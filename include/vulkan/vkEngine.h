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
    void Shutdown() override;
    void ProcessInput() override;

  private:
    vk::raii::Context context;
    vk::raii::Instance instance = nullptr;
    vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr;
    vk::raii::PhysicalDevice physicalDevice = nullptr;
    vk::raii::Device device = nullptr;
    vk::raii::SurfaceKHR surface = nullptr;
    vk::raii::Queue graphicsQueue = nullptr;
    vk::raii::SwapchainKHR swapChain = nullptr;
    std::vector<vk::Image> swapChainImages;
    std::vector<vk::raii::ImageView> swapChainImageViews;
    vk::SurfaceFormatKHR swapChainSurfaceFormat;
    vk::Extent2D swapChainExtent;
    vk::raii::Pipeline graphicsPipeline = nullptr;
    vk::raii::CommandPool commandPool = nullptr;
    uint32_t queueIndex = ~0;
    uint32_t frameIndex = 0;
    std::vector<vk::raii::CommandBuffer> commandBuffers;
    std::vector<vk::raii::Semaphore> presentCompleteSemaphores;
    std::vector<vk::raii::Semaphore> renderFinishedSemaphores;
    std::vector<vk::raii::Fence> inFlightFences;

    void CreateInstance();
    void SetupDebugMessenger();
    void CreateSurface();
    void SelectPhysicalDevice();
    void CreateLogicalDevice();
    void CreateSwapchain();
    void CreateImageViews();
    void CreateGraphicsPipeline();
    void CreateCommandPool();
    void CreateCommandBuffers();
    void DrawFrame();
    void CreateSyncObjects();

    std::vector<const char*> GetRequiredInstanceExtensions();
    bool IsDeviceSuitable(vk::raii::PhysicalDevice const& device);
    vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(std::vector<vk::SurfaceFormatKHR> const &availableFormats);
    vk::Extent2D ChooseSwapExtent(vk::SurfaceCapabilitiesKHR const &surfaceCapabilities);
    uint32_t ChooseSwapMinImageCount(vk::SurfaceCapabilitiesKHR  const &surfaceCapabilities);
    vk::PresentModeKHR ChooseSwapPresentMode(std::vector<vk::PresentModeKHR> const &availablePresentMode);
    [[nodiscard]] vk::raii::ShaderModule CreateShaderModule(const std::vector<char>& code) const;
    void RecordCommandBuffer(uint32_t imageIndex);
    void TransitionImageLayout(uint32_t                imageIndex,
	    vk::ImageLayout         old_layout,
	    vk::ImageLayout         new_layout,
	    vk::AccessFlags2        src_access_mask,
	    vk::AccessFlags2        dst_access_mask,
	    vk::PipelineStageFlags2 src_stage_mask,
	    vk::PipelineStageFlags2 dst_stage_mask);
};