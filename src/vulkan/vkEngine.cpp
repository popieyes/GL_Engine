#include "vulkan/vkEngine.h"
#include <vector>
#include <ranges>
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <iterator>
#include <string>

const std::vector<char const*> validationLayers = {
  "VK_LAYER_KHRONOS_validation"
};
std::vector<const char*> requiredDeviceExtension = {vk::KHRSwapchainExtensionName};
#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif

static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT       severity,
  vk::DebugUtilsMessageTypeFlagsEXT              type,
  const vk::DebugUtilsMessengerCallbackDataEXT * pCallbackData,
  void *                                         pUserData)
{
  std::cerr << "validation layer: type " << to_string(type) << " msg: " << pCallbackData->pMessage << std::endl;

  return vk::False;
}

const int VKEngine::Setup() {
	
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); /* Specify to not create a OpenGL context */
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); /* Windo not Resizable */

  window = glfwCreateWindow(window_settings.width, window_settings.height, window_settings.title.c_str(), NULL, NULL);
	
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
    return 1;
	}

  

  CreateInstance();
  SetupDebugMessenger();
  CreateSurface();
  SelectPhysicalDevice();
  CreateLogicalDevice();

  return 0;
}

void VKEngine::Run() {

}

void VKEngine::BeginFrame() {

}

void VKEngine::EndFrame() {

}


std::vector<const char*> VKEngine::GetRequiredInstanceExtensions()
{
  // Get required instance extensions from GLFW
  uint32_t glfwExtensionCount = 0;
  auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
  // Check if extensions are supported by Vulkan implementation
  std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

  if(enableValidationLayers)
  {
    extensions.push_back(vk::EXTDebugUtilsExtensionName);
  }

  return extensions;
}
void VKEngine::CreateInstance() {
  constexpr vk::ApplicationInfo appInfo{.pApplicationName = "Hernan Engine",
                                        .applicationVersion = VK_MAKE_VERSION(1,0,0),
                                        .pEngineName        = "Hernan Engine",
                                        .engineVersion      = VK_MAKE_VERSION(1,0,0),
                                        .apiVersion         = vk::ApiVersion14};
  
  auto requiredExtensions = GetRequiredInstanceExtensions();

  // Check if extensions are supported by Vulkan implementation
  auto extensionProperties = context.enumerateInstanceExtensionProperties();
  auto unsupportedPropertyIt = std::ranges::find_if(requiredExtensions,
      [&extensionProperties](auto const &requiredExtension) {
        return std::ranges::none_of(extensionProperties, [requiredExtension](auto const &extensionProperty) 
      { return strcmp(extensionProperty.extensionName, requiredExtension) == 0; });
      });

  if (unsupportedPropertyIt != requiredExtensions.end())
  {
    throw std::runtime_error("Required extension not supported: " + std::string(*unsupportedPropertyIt));
  }
  
  // Validation layers
  std::vector<char const*> requiredLayers;
  if(enableValidationLayers)
  {
    requiredLayers.assign(validationLayers.begin(), validationLayers.end());
  }

  // Check if required layers are supported by the Vulkan implementation
  auto layerProperties = context.enumerateInstanceLayerProperties();
  auto unsupportedLayerIt = std::ranges::find_if(requiredLayers,
                                              [&layerProperties](auto const& requiredLayer) {
                                                return std::ranges::none_of(layerProperties,
                                                  [requiredLayer] (auto const& layerProperty) 
                                                  { return strcmp(layerProperty.layerName, requiredLayer) == 0;});
                                              });
  if (unsupportedLayerIt != requiredLayers.end())
  {
    throw std::runtime_error("Required layer not supported: " + std::string(*unsupportedLayerIt));

  }

  vk::InstanceCreateInfo createInfo{
    .pApplicationInfo = &appInfo,
    .enabledLayerCount = static_cast<uint32_t>(requiredLayers.size()),
    .ppEnabledLayerNames = requiredLayers.data(),
    .enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size()),
    .ppEnabledExtensionNames = requiredExtensions.data()
  };

  instance = vk::raii::Instance(context, createInfo);
}

void VKEngine::SetupDebugMessenger() {
  if (!enableValidationLayers) return;

  vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
  vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);
  vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfoEXT{.messageSeverity = severityFlags,
                        .messageType     = messageTypeFlags,
                        .pfnUserCallback = &debugCallback};
  debugMessenger = instance.createDebugUtilsMessengerEXT( debugUtilsMessengerCreateInfoEXT );
}

void VKEngine::SelectPhysicalDevice() {
  
  std::vector<vk::raii::PhysicalDevice> physicalDevices = instance.enumeratePhysicalDevices();
  auto const devIter = std::ranges::find_if( physicalDevices, [&]( auto const & physicalDevice) {return IsDeviceSuitable(physicalDevice);});

  if(devIter == physicalDevices.end())
  {
    throw std::runtime_error( "Failed to find a suitable GPU");
  }
  physicalDevice = *devIter;
  
}

bool VKEngine::IsDeviceSuitable(vk::raii::PhysicalDevice const& physicalDevice)
  {
    auto deviceProperties = physicalDevice.getProperties();
 
    // Check if any of the queue families support graphics operations
    auto queueFamilies    = physicalDevice.getQueueFamilyProperties();
    bool supportsGraphics = std::ranges::any_of( queueFamilies, []( auto const & qfp ) { return !!( qfp.queueFlags & vk::QueueFlagBits::eGraphics ); } );

    bool supportsVulkan1_3 = deviceProperties.apiVersion >= vk::ApiVersion13;
    auto availableDeviceExtensions = physicalDevice.enumerateDeviceExtensionProperties();
    bool supportsAllRequiredExtensions =
      std::ranges::all_of( requiredDeviceExtension,
                           [&availableDeviceExtensions]( auto const & requiredDeviceExtension )
                           {
                             return std::ranges::any_of( availableDeviceExtensions,
                                                         [requiredDeviceExtension]( auto const & availableDeviceExtension )
                                                         { return strcmp( availableDeviceExtension.extensionName, requiredDeviceExtension ) == 0; } );
                           } );
    auto features =
    physicalDevice
      .template getFeatures2<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan13Features, vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();
    bool supportsRequiredFeatures = features.template get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering &&
      features.template get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().extendedDynamicState;


    return supportsVulkan1_3 && supportsGraphics && supportsAllRequiredExtensions && supportsRequiredFeatures;
  }

  void VKEngine::CreateLogicalDevice()
  {
    std::vector<vk::QueueFamilyProperties> queueFamilyProperties = physicalDevice.getQueueFamilyProperties();

    uint32_t queueIndex = ~0;
    for (uint32_t qfpIndex = 0; qfpIndex < queueFamilyProperties.size(); qfpIndex++)
    {
      if ((queueFamilyProperties[qfpIndex].queueFlags & vk::QueueFlagBits::eGraphics) &&
        physicalDevice.getSurfaceSupportKHR(qfpIndex, *surface))
        {
          // found a queue family that supports both graphics and present
          queueIndex = qfpIndex;
          break;
        }
    }
    if (queueIndex == ~0)
    {
      throw std::runtime_error("Could not find a queue for graphics and present -> terminating");
    }

    // Create a chain of feature structures
    vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan13Features, vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT> featureChain = {
        {},                               // vk::PhysicalDeviceFeatures2 (empty for now)
        {.dynamicRendering = true },      // Enable dynamic rendering from Vulkan 1.3
        {.extendedDynamicState = true }   // Enable extended dynamic state from the extension
    };

    float queuePriority = 0.5f;
    vk::DeviceQueueCreateInfo deviceQueueCreateInfo { .queueFamilyIndex = queueIndex, .queueCount = 1, .pQueuePriorities = &queuePriority };
    vk::DeviceCreateInfo deviceCreateInfo {
      .pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
      .queueCreateInfoCount = 1,
      .pQueueCreateInfos = &deviceQueueCreateInfo,
      .enabledExtensionCount = static_cast<uint32_t>(requiredDeviceExtension.size()),
      .ppEnabledExtensionNames = requiredDeviceExtension.data()
    };

    device = vk::raii::Device( physicalDevice, deviceCreateInfo);
    graphicsQueue = vk::raii::Queue(device, queueIndex,0);
  }

  void VKEngine::CreateSurface() 
  {
    VkSurfaceKHR _surface;
    if (glfwCreateWindowSurface(*instance, window, nullptr, &_surface) != 0) {
      throw std::runtime_error("Failed to create window surface!");
    }
    surface = vk::raii::SurfaceKHR(instance, _surface);
  }