#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <optional>
#include <vector>

class VkDeviceManager
{
    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily_m;
        std::optional<uint32_t> presentFamily_m;
        inline bool isComplete();
    };
    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

public:
    // relevant to physicaldevice
    void pickPhysicalDevice(VkInstance& instance);
    // relevant to logical device
    void createLogicalDevice
        (const bool& enableValidationLayers, const std::vector<const char*>& validationLayers);
    void createSurface(const VkInstance& instance, GLFWwindow* window);
    void deviceCleanup(const VkInstance& instance);
    
private:
    QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice& device);
    bool isDeviceSuitable(const VkPhysicalDevice& device);
    // check for swap chain extension
    bool checkDeviceExtensionSupport(const VkPhysicalDevice& device);
    auto querySwapChainSupport(const VkPhysicalDevice& device);
    // implicitly destroyed when vkInstance is destroyed
    VkPhysicalDevice physicalDevice_m = VK_NULL_HANDLE;
    // logical device
    VkDevice device_m;
    VkQueue graphicsQueue_m;
    // window surface
    VkSurfaceKHR surface_m;
    VkQueue presentQueue_m;
    // requied extensions name
    std::vector<const char*> deviceExtensions_m =
    {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
};