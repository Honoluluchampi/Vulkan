#pragma once
#include <vector>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <memory>
#include <functional>
#include <VkDebugger.hpp>
#include <VkDeviceManager.hpp>
#include <VKSwapChainManager.hpp>
#include <VkGraphicsPipeline.hpp>
#include <VkRenderPass.hpp>
#include <VkFramebufferFactory.hpp>
#include <VkCommandManager.hpp>
#include <VkRenderer.hpp>

class Application
{
    template <class T>
    using uptr_t = std::unique_ptr<T>;
    enum class VkStage
    {
        VK_INSTANCE,
        VK_DEBUGGER,
        VK_DEVICE_MANAGER,
        VK_SWAP_CHAIN_MANAGER,
        VK_GRAPHICS_PIPELINE,
        VK_FRAME_BUFFER,
        VK_COMMAND_BUFFER,
        VK_RENDERER
    };
    struct VkCreateFunc
    {
        VkStage stage_m;
        std::function<void(void)>func_m;
        VkCreateFunc(VkStage stage, std::function<void(void)>func)
            :stage_m(stage), func_m(func) {}
    };
    //struct VkDestroyFunc;
public:
    Application();
    void  run();
private:
    // functions
    void initWindow();
    void initVulkan();
    // for initializing vulkan
    void initCreateFunctions();
    void initDestroyFunctions();
    void execCreateFunctions();
    void mainLoop();
    void cleanup();
    void checkingForExtensionSupport();
    bool checkValidationLayerSupport();
    void createInstance();
    void recreateSwapChain();
    // message callback
    std::vector<const char*> getRequiredExtensions();
    // up's communication
    const VkDeviceManager& getDeviceManagerRef() const;
    const VkSwapChainManager& getSwapChainManagerRef() const;
    // variables
    GLFWwindow* window_m;
    VkInstance instance_m;
    std::vector<const char*> validationLayers_m;
    bool enableValidationLayers_m;
    // original debugger
    uptr_t<VkDebugger> upDebugger_m;
    uptr_t<VkDeviceManager> upDeviceManager_m;
    uptr_t<VkSwapChainManager> upSwapChainManager_m;
    uptr_t<VkGraphicsPipelineFactory> upGraphicsPipeline_m;
    uptr_t<VkFramebufferFactory> upFramebufferFactory_m;
    uptr_t<VkCommandManager> upCommandManager_m;
    uptr_t<VkRenderer> upRenderer_m;
    // createXX functions
    std::vector<struct Application::VkCreateFunc> createFunctions_m;
    // destroyXX functions
    //std::vector<VkDestroy Func> destroyFunctions_m;
};