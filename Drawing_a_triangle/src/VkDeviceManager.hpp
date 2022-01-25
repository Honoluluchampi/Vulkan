#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class VkDeviceManager
{
public:
    void pickPhysicalDevice(VkInstance& instance);
    bool isDeviceSuitable(const VkPhysicalDevice& device);
private:
    // implicitly destroyed when vkInstance is destroyed
    VkPhysicalDevice physicalDevice_m = VK_NULL_HANDLE;
};