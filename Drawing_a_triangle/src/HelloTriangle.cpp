#define GLFW_INCLUDE_VULKAN // by putting this, vulkan.h is included in glfw3.h
#include <GLFW/glfw3.h>
//#include <vulkan/vulkan.h> // functions, structures, enumerations from LunarG SDK
#include <iostream>     
#include <stdexcept> // report and propagate an error
#include <cstdlib> // EXIT_SUCCESS, EXIT_FAILURE
#include <cstring>
#include <vector>
#include "HelloTriangle.hpp"

constexpr uint32_t WIDTH = 800;
constexpr uint32_t HEIGHT = 600;

// init app's information variables
HelloTriangleApplication::HelloTriangleApplication()
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

void HelloTriangleApplication::run()
{
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
}
// init GLFW, create  window
void HelloTriangleApplication::initWindow()
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
void HelloTriangleApplication::initVulkan()
{
    createInstance();
    setupDebugMessenger();
}

void HelloTriangleApplication::mainLoop()
{
    while (!glfwWindowShouldClose(window_m)){
        glfwPollEvents();
    }
}

void HelloTriangleApplication::cleanup()
{
    if (enableValidationLayers_m)
        DestroyDebugUtilsMessengerEXT(instance_m, debugMessenger_m, nullptr);
    vkDestroyInstance(instance_m, nullptr);
    // once window_m is closed, destroy resources and terminate glfw
    glfwDestroyWindow(window_m);
    glfwTerminate();
}
// fill in a struct with some informattion about the application
void HelloTriangleApplication::createInstance()
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
    if (enableValidationLayers_m) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers_m.size());
        createInfo.ppEnabledLayerNames = validationLayers_m.data();
    }
    else createInfo.enabledLayerCount = 0;
    // 1st : pointer to struct with creation info
    // 2nd : pointer to custom allocator callbacks
    // 3rd : pointer to the variable that stores the handle to the new object
    if (vkCreateInstance(&createInfo, nullptr, &instance_m) != VK_SUCCESS)
        throw std::runtime_error("failed to create instance!");
}

void HelloTriangleApplication::checkingForExtensionSupport()
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
bool HelloTriangleApplication::checkValidationLayerSupport() {
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
std::vector<const char*> HelloTriangleApplication::getRequiredExtensions()
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

VKAPI_ATTR VkBool32 VKAPI_CALL HelloTriangleApplication::debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void *pUserData)
{
    std::cerr << "validation layer:" << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}
// fix this function to control debug call back of the apps
void HelloTriangleApplication::setupDebugMessenger()
{
    if(!enableValidationLayers_m) return;
    // fill in a structure with details about the meszsenger and its callback
    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    createInfo.sType = 
        VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = 
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    // Pointer to FunctionN
    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = nullptr; // optional
}

VkResult HelloTriangleApplication::VkCreateDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)
        // create the extension object if its available
        // Since the debug messenger is specific to our Vulkan instance and its layers, it needs to be explicitly specified as first argument.
        vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void HelloTriangleApplication::DestroyDebugUtilsMessengerEXT(
    VkInstance instance,
    VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)
        vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    // execute  destruction
    if (func != nullptr) func(instance, debugMessenger, pAllocator);
}