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
        INSTANCE,
        DEBUGGER,
        SURFACE,
        PHYSICAL_DEVICE,
        LOGICAL_DEVICE,
        SWAP_CHAIN,
        IMAGE_VIEWS,
        RENDER_PASS,
        GRAPHICS_PIPELINE,
        FRAME_BUFFERS,
        COMMAND_POOL,
        COMMAND_BUFFER,
        RENDERER
    };
    struct VkStageFunc
    {
        VkStage stage_m;
        std::function<void(void)>func_m;
        VkStageFunc(VkStage stage, std::function<void(void)>func)
            :stage_m(stage), func_m(func) {}
    };
    //struct VkDestroyFunc;
public:
    Application();
    void run();
private:
    // functions
    void initWindow();
    void initVulkan();
    // for initializing vulkan
    void initCreateFunctions();
    void initDestroyFunctions();
    void execFunctionsSequence
        (const std::vector<VkStageFunc>& targetFuncs, const std::vector<VkStage>& stages = {});
    void mainLoop();
    void cleanup();
    void checkingForExtensionSupport();
    bool checkValidationLayerSupport();
    void createInstance();
    void recreateSwapChain();
    // handle framebuffer resizes
    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
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
    VkDebugger debugger_m;
    VkDeviceManager deviceManager_m;
    VkSwapChainManager swapChainManager_m;
    VkGraphicsPipelineFactory graphicsPipeline_m;
    VkFramebufferFactory framebufferFactory_m;
    VkCommandManager commandManager_m;
    VkRenderer renderer_m;
    // createXX functions
    std::vector<VkStageFunc> createFunctions_m;
    // destroyXX functions
    std::vector<VkStageFunc> destroyFunctions_m;
};