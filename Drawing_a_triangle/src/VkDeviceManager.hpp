#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <optional>

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
    void createLogicalDevice();
    
private:
    bool isDeviceSuitable(const VkPhysicalDevice& device);
    // implicitly destroyed when vkInstance is destroyed
    VkPhysicalDevice physicalDevice_m = VK_NULL_HANDLE;
    VkDevice divice_m;
};