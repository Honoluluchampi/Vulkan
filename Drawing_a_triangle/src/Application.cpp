//#include <vulkan/vulkan.h> // functions, structures, enumerations from LunarG SDK
#include <iostream>     
#include <stdexcept> // report and propagate an error
#include <cstdlib> // EXIT_SUCCESS, EXIT_FAILURE
#include <cstring>
#include <vector>
#include <Application.hpp>
#include <memory>

constexpr uint32_t WIDTH = 800;
constexpr uint32_t HEIGHT = 600;

// init app's information variables
Application::Application() : deviceManager_m(), swapChainManager_m(getDeviceManagerRef(), window_m)
{
    validationLayers_m = {
    "VK_LAYER_KHRONOS_validation"
    };
    #ifdef NDEBUG
        enableValidationLayers_m = false;
    #else
        enableValidationLayers_m = true;
    #endif
}

void Application::run()
{
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
}
// init GLFW, create  window
void Application::initWindow()
{
    glfwInit();
    // disable openGL
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    //glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    // create actual window
    // 4th arg : choosing monitor, 5th : only relevant to openGL
    window_m = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    // handle framebuffer resizes
    glfwSetWindowUserPointer(window_m, this);
    glfwSetFramebufferSizeCallback(window_m, framebufferResizeCallback);
}
// init validation layer, instance
void Application::initVulkan()
{
    initCreateFunctions();
    // initDestroyFunctions();
    execFunctionsSequence(createFunctions_m);
}

void Application::execFunctionsSequence
    (const std::vector<VkStageFunc>& targetFuncs, const std::vector<VkStage>& stages)
{
    // execute all create functions
    if(stages.size() == 0)
        for(const auto& func : targetFuncs)
            func.func_m();
    // execute specified create functions
    else {
        auto cursor = targetFuncs.begin();
        for(const auto& stage : stages) {
            // dont exceed the end of the function list
            while(cursor != targetFuncs.end() && stage != cursor->stage_m) 
                cursor++;
            if (cursor == targetFuncs.end())
                std::runtime_error("there is an invalid stage or dependency problem.");
            else 
                cursor->func_m();
        }
    }
}

void Application::mainLoop()
{
    while (!glfwWindowShouldClose(window_m)){
        glfwPollEvents();
        bool swapChainUpToDate = renderer_m.drawFrame
        (
            getDeviceManagerRef(),
            swapChainManager_m.getSwapChainRef(),
            commandManager_m.getCommandBufferRef()
        );
        if(!swapChainUpToDate)
            recreateSwapChain();
    }
    // wait for the logical device to finish operations 
    // before exiting mainLoop and destroying the windwo
    vkDeviceWaitIdle(deviceManager_m.getDevice());
}

void Application::cleanup()
{
    // once window_m is closed, destroy resources and terminate glfw
    execFunctionsSequence(destroyFunctions_m);
    glfwDestroyWindow(window_m);
    glfwTerminate();
}
// fill in a struct with some informattion about the application
void Application::createInstance()
{
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    checkingForExtensionSupport();
    // getting required extentions according to whether debug mode or not
    auto extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount
        = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();
    // validation layers
    if (enableValidationLayers_m && !checkValidationLayerSupport()) {
        throw std::runtime_error("validation layers requested, but not available!");
    }
    // additional debugger for vkCreateInstance and vkDestroyInstance
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
    if (enableValidationLayers_m) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers_m.size());
        createInfo.ppEnabledLayerNames = validationLayers_m.data();
        VkDebugger::populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
    }
    else {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }
    // 1st : pointer to struct with creation info
    // 2nd : pointer to custom allocator callbacks
    // 3rd : pointer to the variable that stores the handle to the new object
    if (vkCreateInstance(&createInfo, nullptr, &instance_m) != VK_SUCCESS)
        throw std::runtime_error("failed to create instance!");
}

void Application::checkingForExtensionSupport()
{
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    // each VkExtensionProperties contains the name and version of an extension
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
    std::cout << "available extensions: \n";
    for (const auto &extension : extensions) {
        std::cout << '\t' << extension.extensionName << '\n';
    }
    // check wheather all glfwExtensions are supported
}
// same as chckingForExtensionSupport()
bool Application::checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
    // output avaliable and required layers
    std::cout << "available layers :" << std::endl;
    for (const auto &layerName : availableLayers) 
        std::cout << "\t" << layerName.layerName << std::endl;
    std::cout << "required layers :" << std::endl;
    for (const auto  &layerName : validationLayers_m)
        std::cout << "\t"  <<  layerName << std::endl;
    // check if all of the layers in validationLayers exist in the availableLyaers
    for (const char *layerName : validationLayers_m){
        bool  layerFound = false;
        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }
        if (!layerFound) {
            return false;
        }
    }
    return true;
}
// required list of extensions based on wheather validation lyaers are enabled
std::vector<const char*> Application::getRequiredExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions
        = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    // convert const char** to std::vector<const char*>
    std::vector<const char*> extensions(glfwExtensions, 
        glfwExtensions + glfwExtensionCount); // ???
    if(enableValidationLayers_m) {
        // add debug messenger extension
        // VK_EXT_DEBUG_UTILS_EXTENSION_NAME = VK_EXT_debug_utils (literal string)
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    std::cout << "required extensions :"  << std::endl;
    for(const auto& extensions : extensions)
        std::cout << "\t" << extensions << std::endl;
    return extensions;
}

void Application::recreateSwapChain()
{
    // stop calculation until the window is minimized
    int width = 0, height = 0;
    glfwGetFramebufferSize(window_m, &width, &height);
    while(width == 0 || height ==0) {
        glfwGetFramebufferSize(window_m, &width, &height);
        glfwWaitEvents();
    }

    // wait for finishing the current task
    vkDeviceWaitIdle(deviceManager_m.getDevice());
    // recreation
    // recreate anything dependent on the swap chain
    std::vector<VkStage> stageList = 
    {
        VkStage::COMMAND_BUFFER,
        VkStage::FRAME_BUFFERS,
        VkStage::GRAPHICS_PIPELINE,
        VkStage::RENDER_PASS,
        VkStage::IMAGE_VIEWS,
        VkStage::SWAP_CHAIN
    };
    // destroy anything to recreate right now
    execFunctionsSequence(destroyFunctions_m, stageList);
    std::reverse(stageList.begin(), stageList.end());
    execFunctionsSequence(createFunctions_m, stageList);
}

void Application::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    auto app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
    app->renderer_m.framebufferResized = true;
}

void Application::initCreateFunctions()
{
    createFunctions_m.emplace_back
        (VkStage::INSTANCE, [this]()
            {
                // window surface should be created right after the
                // instance creation, because it can actually influence the physical device selection
                createInstance();
            });
    createFunctions_m.emplace_back
        (VkStage::DEBUGGER, [this]()
            {
                if(enableValidationLayers_m){
                    // Allocate debugger 
                    debugger_m.setupDebugMessenger(instance_m);
                }
            });
    createFunctions_m.emplace_back
        (VkStage::SURFACE, [this]()
            {
                deviceManager_m.createSurface(instance_m, window_m);
            });
    createFunctions_m.emplace_back
        (VkStage::PHYSICAL_DEVICE, [this]()
            {
                deviceManager_m.pickPhysicalDevice(instance_m);
            });
    createFunctions_m.emplace_back
        (VkStage::LOGICAL_DEVICE, [this]()
            {
                deviceManager_m.createLogicalDevice(enableValidationLayers_m, validationLayers_m);
            });
    createFunctions_m.emplace_back
        (VkStage::SWAP_CHAIN, [this]()
            {
                swapChainManager_m.createSwapChain(WIDTH, HEIGHT);
            });
    createFunctions_m.emplace_back
        (VkStage::IMAGE_VIEWS, [this]()
            {
                swapChainManager_m.createImageViews(); 
            });
    createFunctions_m.emplace_back
        (VkStage::RENDER_PASS, [this]()
            {
                graphicsPipeline_m.createRenderPass
                (
                    deviceManager_m.getDevice(),
                    swapChainManager_m.getSwapChainImageFormatRef()
                );
            });
    createFunctions_m.emplace_back
        (VkStage::VERTEX_FACTORY, [this]()
            {
               vertexManager_m.createVerticesData();
            });
    createFunctions_m.emplace_back
        (VkStage::GRAPHICS_PIPELINE, [this]()
            {
                graphicsPipeline_m.createGraphicsPipeline
                (
                    deviceManager_m.getDevice(),
                    swapChainManager_m.getSwapChainExtentRef()
                );
            });
    createFunctions_m.emplace_back
        (VkStage::FRAME_BUFFERS, [this]()
            {
                framebufferFactory_m.createFramebuffers
                (
                    deviceManager_m.getDevice(), 
                    getSwapChainManagerRef(),
                    graphicsPipeline_m.getRenderPassRef()
                );
            });
    createFunctions_m.emplace_back
        (VkStage::COMMAND_POOL, [this]()
            {
                commandManager_m.createCommandPool(getDeviceManagerRef());
            });
    createFunctions_m.emplace_back
        (VkStage::VERTEX_BUFFER, [this]()
            {
                vertexManager_m.createVertexBuffer
                (
                    deviceManager_m.getDevice(),
                    deviceManager_m.getPhysicalDevice()
                );
            });
    createFunctions_m.emplace_back
        (VkStage::COMMAND_BUFFER, [this]()
            {
                commandManager_m.createCommandBuffers
                (
                    deviceManager_m.getDevice(), 
                    graphicsPipeline_m.getRenderPassRef(), 
                    framebufferFactory_m.getSwapChainFrameBuffersRef(), 
                    graphicsPipeline_m.getGraphicsPipelineRef(),
                    swapChainManager_m.getSwapChainExtentRef(),
                    vertexManager_m.getVertexBufferRef(),
                    vertexManager_m.getVerticesSize()
                );
            });
    createFunctions_m.emplace_back
        (VkStage::RENDERER, [this]()
            {
                renderer_m.createSyncObjects
                (
                    deviceManager_m.getDevice(),
                    swapChainManager_m.getSwapChainImagesNum()
                );
            });
}

void Application::initDestroyFunctions()
{
    destroyFunctions_m.emplace_back
        (VkStage::RENDERER, [this]()
        {
            renderer_m.destroyRenderer(getDeviceManagerRef().getDevice());
        });
    destroyFunctions_m.emplace_back
        (VkStage::COMMAND_BUFFER, [this]()
        {
            commandManager_m.destroyCommandBuffers(getDeviceManagerRef().getDevice());
        });
    destroyFunctions_m.emplace_back
        (VkStage::VERTEX_BUFFER, [this]()
        {
            vertexManager_m.destroyVertexBuffer(deviceManager_m.getDevice());
        });
    destroyFunctions_m.emplace_back
        (VkStage::COMMAND_POOL, [this]()
        {
            commandManager_m.destroyCommandPool(getDeviceManagerRef().getDevice());
        });
    destroyFunctions_m.emplace_back
        (VkStage::FRAME_BUFFERS, [this]()
        {
            framebufferFactory_m.destroyFramebuffers(getDeviceManagerRef());
        });
    destroyFunctions_m.emplace_back
        (VkStage::GRAPHICS_PIPELINE, [this]()
        {
            graphicsPipeline_m.destroyGraphicsPipeline(deviceManager_m.getDevice());
        });
    destroyFunctions_m.emplace_back
        (VkStage::RENDER_PASS, [this]
        {
            graphicsPipeline_m.destroyRenderPass(deviceManager_m.getDevice());
        });
    destroyFunctions_m.emplace_back
        (VkStage::IMAGE_VIEWS, [this]
        {
            swapChainManager_m.destroyImageViews();
        });
    destroyFunctions_m.emplace_back
        (VkStage::SWAP_CHAIN, [this]
        {
            swapChainManager_m.destroySwapChain();
        });
    destroyFunctions_m.emplace_back
        (VkStage::LOGICAL_DEVICE, [this]
        {
            deviceManager_m.destroyLogicalDevice(instance_m);
        });
    destroyFunctions_m.emplace_back
        (VkStage::PHYSICAL_DEVICE, [this]
        {
            deviceManager_m.destroyPhysicalDevice(instance_m);
        });
    destroyFunctions_m.emplace_back
        (VkStage::SURFACE, [this]
        {
            deviceManager_m.destroySurface(instance_m);
        });
    destroyFunctions_m.emplace_back
        (VkStage::DEBUGGER, [this]
        {
            if (enableValidationLayers_m)
                debugger_m.destroyDebugUtilsMessengerEXT(instance_m, nullptr);
        });
    destroyFunctions_m.emplace_back
        (VkStage::INSTANCE, [this]
        {
            vkDestroyInstance(instance_m, nullptr);
        });
}

const VkDeviceManager& Application::getDeviceManagerRef() const
    {return deviceManager_m;}

const VkSwapChainManager& Application::getSwapChainManagerRef() const
    {return swapChainManager_m;}