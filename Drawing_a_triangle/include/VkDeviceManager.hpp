#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <optional>
#include <vector>
#include <VKSwapChainManager.hpp>

class VkDeviceManager
{
    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities_m;
        std::vector<VkSurfaceFormatKHR> formats_m;
        std::vector<VkPresentModeKHR> presentModes_m;
    };

    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily_m;
        std::optional<uint32_t> presentFamily_m;
        inline bool isComplete();
    };

public:
    // relevant to physicaldevice
    void pickPhysicalDevice(VkInstance& instance);
    // relevant to logical device
    void createLogicalDevice
        (const bool& enableValidationLayers, const std::vector<const char*>& validationLayers);
    void createSurface(const VkInstance& instance, GLFWwindow* window);
    void deviceCleanup(const VkInstance& instance);
    void createSwapChain();
    void createImageViews();
    // make sure that you dont change the device_m out of this class
    const VkDevice& getDevice() const;
    const VkPhysicalDevice& getPhysicalDevice() const;
    const VkQueue& getGraphicsQueueRef() const;
    const VkQueue& getPresentQueueRef() const;
    QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice& device);
    
private:
    bool isDeviceSuitable(const VkPhysicalDevice& device);
    SwapChainSupportDetails querySwapChainSupport(const VkPhysicalDevice& device);
    // check for swap chain extension
    bool checkDeviceExtensionSupport(const VkPhysicalDevice& device);
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
    friend void VkSwapChainManager::createSwapChain(const VkDeviceManager& deviceManager);
};