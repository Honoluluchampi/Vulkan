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
    if(enableValidationLayers_m){
        // Allocate debugger 
        upDebugger_m.reset(new VkDebugger());
        upDebugger_m->setupDebugMessenger(instance_m);
    }
    pickPhysicalDevice();
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
        upDebugger_m->destroyDebugUtilsMessengerEXT(instance_m, nullptr);
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

bool isDeviceSuitable(const VkPhysicalDevice& device) {
    // VkPhysicalDeviceProperties deviceProperties; 
    // VkPhysicalDeviceFeatures deviceFeatures; 
    // vkGetPhysicalDeviceProperties(device, &deviceProperties); 
    // vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
    // return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU 
    //     && deviceFeatures.geometryShader;
    return true;
}

void HelloTriangleApplication::pickPhysicalDevice()
{
    // rate device suitability if its nesessary
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance_m, &deviceCount, nullptr);
    if (deviceCount == 0){
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }
    // allocate an array to hold all of the VkPhysicalDevice handle
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance_m, &deviceCount, devices.data());
    physicalDevice_m = VK_NULL_HANDLE;
    for (const auto &device : devices) {
        if (isDeviceSuitable(device)) {
            physicalDevice_m = device;
            break;
        }
    }
    if (physicalDevice_m == VK_NULL_HANDLE) {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
}