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
Application::Application()
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
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    // create actual window
    // 4th arg : choosing monitor, 5th : only relevant to openGL
    window_m = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
}
// init validation layer, instance
void Application::initVulkan()
{
    createInstance();
    // window surface should be created right after the
    // instance creation, because it can actually influence the physical device selection
    if(enableValidationLayers_m){
        // Allocate debugger 
        upDebugger_m.reset(new VkDebugger());
        upDebugger_m->setupDebugMessenger(instance_m);
    }
    // device
    upDeviceManager_m.reset(new VkDeviceManager());
    upDeviceManager_m->createSurface(instance_m, window_m);
    upDeviceManager_m->pickPhysicalDevice(instance_m);
    upDeviceManager_m->createLogicalDevice(enableValidationLayers_m, validationLayers_m);
    // swap chain
    upSwapChainManager_m.reset(new VkSwapChainManager(getDeviceManagerRef().getDevice(),WIDTH,HEIGHT));
    upSwapChainManager_m->createSwapChain(getDeviceManagerRef());
    upSwapChainManager_m->createImageViews();
    // graphics pipeline
    upGraphicsPipeline_m.reset(new VkGraphicsPipelineFactory());
    upGraphicsPipeline_m->createGraphicsPipeline
    (
        upDeviceManager_m->getDevice(),
        upSwapChainManager_m->getSwapChainExtentRef(),
        upSwapChainManager_m->getSwapChainImageFormatRef()
    );
    // framebuffer
    upFramebufferFactory_m.reset(new VkFramebufferFactory());
    upFramebufferFactory_m->createFramebuffers
    (
        upDeviceManager_m->getDevice(), 
        getSwapChainManagerRef(),
        upGraphicsPipeline_m->getRenderPassRef()
    );
    // command buffer
    upCommandManager_m.reset(new VkCommandManager());
    upCommandManager_m->createCommandPool(getDeviceManagerRef());
    upCommandManager_m->createCommandBuffers
    (
        upDeviceManager_m->getDevice(), 
        upGraphicsPipeline_m->getRenderPassRef(), 
        upFramebufferFactory_m->getSwapChainFrameBuffersRef(), 
        upGraphicsPipeline_m->getGraphicsPipelineRef(),
        upSwapChainManager_m->getSwapChainExtentRef()
    );
    upRenderer_m.reset(new VkRenderer());
    upRenderer_m->createSyncObjects
    (
        upDeviceManager_m->getDevice(),
        upSwapChainManager_m->getSwapChainImagesNum()
    );
}

void Application::mainLoop()
{
    while (!glfwWindowShouldClose(window_m)){
        glfwPollEvents();
        upRenderer_m->drawFrame
        (
            getDeviceManagerRef(),
            upSwapChainManager_m->getSwapChainRef(),
            upCommandManager_m->getCommandBufferRef()
        );
    }
    // wait for the logical device to finish operations 
    // before exiting mainLoop and destroying the windwo
    vkDeviceWaitIdle(upDeviceManager_m->getDevice());
}

void Application::cleanup()
{
    upRenderer_m->destroyRenderer(getDeviceManagerRef().getDevice());
    upCommandManager_m->destroyCommandPoolandBuffers(getDeviceManagerRef().getDevice());
    // to prevent VkCommandManager.commandBuffers_m from double deleted
    upCommandManager_m.reset();
    upFramebufferFactory_m->destroyFramebuffers(getDeviceManagerRef());
    upGraphicsPipeline_m->destroyGraphicsPipeline(upDeviceManager_m->getDevice());
    upSwapChainManager_m->destroySwapChain();
    // destroy logical device in its destructor
    upDeviceManager_m->deviceCleanup(instance_m);
    if (enableValidationLayers_m)
        upDebugger_m->destroyDebugUtilsMessengerEXT(instance_m, nullptr);
    vkDestroyInstance(instance_m, nullptr);
    // once window_m is closed, destroy resources and terminate glfw
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

const VkDeviceManager& Application::getDeviceManagerRef() const
    {return *upDeviceManager_m;}

const VkSwapChainManager& Application::getSwapChainManagerRef() const
    {return *upSwapChainManager_m;}