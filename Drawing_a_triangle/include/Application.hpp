#pragma once
#include <vector>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <memory>
#include <VkDebugger.hpp>
#include <VkDeviceManager.hpp>
#include <VkGraphicsPipeline.hpp>
#include <VkRenderPass.hpp>
#include <VkFramebufferFactory.hpp>
#include <VkCommandManager.hpp>

class Application
{
    template <class T>
    using uptr_t = std::unique_ptr<T>;
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
    // up's communication
    const VkDeviceManager& getDeviceManagerRef() const;
    // variables
    GLFWwindow* window_m;
    VkInstance instance_m;
    std::vector<const char*> validationLayers_m;
    bool enableValidationLayers_m;
    // original debugger
    uptr_t<VkDebugger> upDebugger_m;
    uptr_t<VkDeviceManager> upDeviceManager_m;
    uptr_t<VkGraphicsPipelineFactory> upGraphicsPipeline_m;
    uptr_t<VkFramebufferFactory> upFramebufferFactory_m;
    uptr_t<VkCommandManager> upCommandManager_m;
};