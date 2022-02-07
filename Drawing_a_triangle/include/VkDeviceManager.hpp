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
        VkSurfaceCapabilitiesKHR capabilities_m;
        std::vector<VkSurfaceFormatKHR> formats_m;
        std::vector<VkPresentModeKHR> presentModes_m;
    };

public:
    VkDeviceManager(const uint32_t width, const uint32_t height)
        : swapChainExtent_m{width, height}{}
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
    VkDevice* getDeviceRefference();
    
private:
    QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice& device);
    bool isDeviceSuitable(const VkPhysicalDevice& device);
    // check for swap chain extension
    bool checkDeviceExtensionSupport(const VkPhysicalDevice& device);
    SwapChainSupportDetails querySwapChainSupport(const VkPhysicalDevice& device);
    // choosing the right settings for the swap chain
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const
        std::vector<VkSurfaceFormatKHR>& availableFormats);
    // most important settings for swap chain
    VkPresentModeKHR chooseSwapPresentMode(const
        std::vector<VkPresentModeKHR>& availablePresentModes);
    // choose resolution of output
    VkExtent2D chooseSwapExtent(const
        VkSurfaceCapabilitiesKHR& capabilities);

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
    // swap chain
    VkSwapchainKHR swapChain_m;
    // handles of the VkImages in the swap chain
    std::vector<VkImage> swapChainImages_m;
    VkFormat swapChainImageFormat_m;
    VkExtent2D swapChainExtent_m;
    // image views
    std::vector<VkImageView> swapChainImageViews_m;
};