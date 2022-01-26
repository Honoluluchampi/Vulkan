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
        inline bool isComplete();
    };
public:
    // relevant to physicaldevice
    void pickPhysicalDevice(VkInstance& instance);
    QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice& device);
    // relevant to logical device
    void createLogicalDevice
        (const bool& enableValidationLayers, const std::vector<const char*>& validationLayers);
    void createSurface(const VkInstance& instance, GLFWwindow* window);
    void deviceCleanup(const VkInstance& instance);
    
private:
    bool isDeviceSuitable(const VkPhysicalDevice& device);
    // implicitly destroyed when vkInstance is destroyed
    VkPhysicalDevice physicalDevice_m = VK_NULL_HANDLE;
    // logical device
    VkDevice device_m;
    VkQueue graphicsQueue_m;
    // window surface
    VkSurfaceKHR surface_m;
};