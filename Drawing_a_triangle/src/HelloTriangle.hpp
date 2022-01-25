#pragma once
#include <vector>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <memory>
#include "VkDebugger.hpp"

class HelloTriangleApplication
{
public:
    HelloTriangleApplication();
    void  run();
private:
    // functions
    void initWindow();
    void initVulkan();
    void mainLoop();
    void cleanup();
    void checkingForExtensionSupport();
    bool checkValidationLayerSupport();
    void createInstance();
    void pickPhysicalDevice();
    // message callback
    std::vector<const char*> getRequiredExtensions();
    // variables
    GLFWwindow* window_m;
    VkInstance instance_m;
    std::vector<const char*> validationLayers_m;
    bool enableValidationLayers_m;
    // original debugger
    std::unique_ptr<VkDebugger> upDebugger_m;
    // implicitly destroyed when vkInstance is destroyed
    VkPhysicalDevice physicalDevice_m;
};