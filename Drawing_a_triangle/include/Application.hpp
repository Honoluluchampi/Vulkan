#pragma once
#include <vector>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <memory>
#include "VkDebugger.hpp"
#include "VkDeviceManager.hpp"
#include "VkGraphicsPipeline.hpp"

class Application
{
public:
    Application();
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
    // message callback
    std::vector<const char*> getRequiredExtensions();
    // variables
    GLFWwindow* window_m;
    VkInstance instance_m;
    std::vector<const char*> validationLayers_m;
    bool enableValidationLayers_m;
    // original debugger
    std::unique_ptr<VkDebugger> upDebugger_m;
    std::unique_ptr<VkDeviceManager> upDeviceManager_m;
    std::unique_ptr<VkGraphicsPipeline> upGraphicsPipeline_m;
};